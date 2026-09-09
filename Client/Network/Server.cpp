#include <RakPeer.h>
#include <MessageIdentifiers.h>
#include "Server.h"
#include "IdManager.h"
#include "NetworkSettings.h"
#include "API.h"
#include "util/Http.h"
#include "util/standardout.h"

static RBX::Reflection::BoundFuncDesc<RBX::Network::Server, void(int, int), 2> server_startFunction(&RBX::Network::Server::start, "Start", "port", "threadSleepTime", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Server, void(int), 1>  f_disconnect(&RBX::Network::Server::stop, "Stop", "blockDuration", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Server, int(void), 0> f_GetClientCount(&RBX::Network::Server::getClientCount, "GetClientCount", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

static RBX::Reflection::BoundFuncDesc<RBX::Network::Server, void(std::string, std::string, int), 3> server_setServerManagerPingFunction(&RBX::Network::Server::setServerManagerPing, "SetServerManagerPing", "pingUrl", "publicIP", "thumbnailId", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

static bool isReplicator(boost::shared_ptr<RBX::Instance> instance)
{
	return dynamic_cast<const RBX::Network::Replicator*>(instance.get()) != NULL;
}

namespace RBX
{
	namespace Network
	{
		Server::Server()
			: outgoingPort(0)
		{
			setName("NetworkServer");
			rakPeer->SetMaximumIncomingConnections(32);
			rakPeer->SetIncomingPassword(API::version.c_str(), (int)API::version.size());
			updateLogger();
		}

		Server::~Server() {}

		void Server::stop(int blockDuration)
		{
			if (rakPeer->IsActive())
				rakPeer->Shutdown(blockDuration);

			removeAllChildren();
		}

		void Server::start(int port, int threadSleepTime)
		{
			SocketDescriptor d(port, "");

			if (!rakPeer->Startup(32, threadSleepTime, &d, 1))
				throw std::runtime_error("Failed to start network server");

			outgoingPort = port;
			StandardOut::singleton()->print(MESSAGE_INFO, "Starting network server on port %d", port);

			unsigned numAddresses = rakPeer->GetNumberOfAddresses();

			StandardOut::singleton()->print(MESSAGE_INFO, "IP addresses:");

			for (unsigned i = 0; i < numAddresses; i++)
			{
				StandardOut::singleton()->print(MESSAGE_INFO, "%s", rakPeer->GetLocalIP(i));
			}

			updateNetworkSimulator();
		}

		int Server::getClientCount()
		{
			if (getChildren())
			{
				return std::count_if(getChildren()->begin(), getChildren()->end(), &isReplicator);
			}
			else
			{
				return 0;
			}
		}

		void Server::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
		{
			if (oldProvider)
			{
				players->setConnection(NULL);

				if (rakPeer->IsActive())
					rakPeer->Shutdown(1000);

				removeAllChildren();

				players.reset();
			}

			Peer::onServiceProvider(oldProvider, newProvider);

			if (newProvider)
			{
				players = shared_from(newProvider->create<Players>());

				players->setConnection(peerInterface());
			}
		}

		void Server::setServerManagerPing(std::string pingUrl, std::string publicIP, int thumbnailId)
		{
			pingThread.reset(new worker_thread(boost::bind(&Server::ping, boost::weak_ptr<Server>(shared_from(this)), publicIP, thumbnailId, pingUrl), "rbx_serverping"));
		}

		bool Server::serverIsPresent(const Instance* context, bool testInDatamodel)
		{
			const ServiceProvider* sp = ServiceProvider::findServiceProvider(context);
			RBXASSERT(!testInDatamodel || sp);

			return ServiceProvider::find<Server>(sp) != NULL;
		}

		PluginReceiveResult Server::OnReceive(RakPeerInterface* peer, Packet* packet)
		{
			PluginReceiveResult result = PluginInterface::OnReceive(peer, packet);
			if (result != RR_CONTINUE_PROCESSING)
				return result;

			if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				try
				{
					StandardOut::singleton()->print(MESSAGE_INFO, "New connection from %s\n", packet->systemAddress.ToString());

					boost::shared_ptr<ClientProxy> proxy = Creatable::create<ClientProxy>(packet->systemAddress, this);
					proxy->setParent(this);
					proxy->sendTop();

					event_IncommingConnection.fire(this, packet->systemAddress.ToString(), proxy);
				}
				catch (std::exception& e)
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Client::OnReceive packet %d: %s", packet->data[0], e.what());
				}
			}

			return RR_CONTINUE_PROCESSING;
		}

		worker_thread::work_result Server::ping(boost::weak_ptr<Server> server, std::string publicIP, int thumbnailId, std::string pingUrl)
		{
			try
			{
				std::stringstream nameStream;

				{
					TextXmlWriter writer(nameStream);

					XmlElement root(Name::declare("root", -1));

					boost::shared_ptr<Server> s = server.lock();
					if (!s)
						return worker_thread::done;

					root.addChild(new XmlElement(Name::declare("server", -1), publicIP));
					root.addChild(new XmlElement(Name::declare("port", -1), s->outgoingPort));
					root.addChild(new XmlElement(Name::declare("numPlayers", -1), s->players->numPlayers()));
					root.addChild(new XmlElement(Name::declare("maxPlayers", -1), s->players->getMaxPlayers()));
					root.addChild(new XmlElement(Name::declare("thumbnailId", -1), thumbnailId));

					writer.serialize(&root);

					nameStream.flush();
				}

				std::string response;
				Http(pingUrl).post(nameStream, false, response);
			}
			catch (std::exception& exp)
			{
				StandardOut::singleton()->print(MESSAGE_ERROR, exp);
			}

			boost::xtime xt;
			boost::xtime_get(&xt, boost::TIME_UTC);

			xt.sec += 10;

			boost::thread::sleep(xt);

			return worker_thread::more;
		}

		Server::ClientProxy::ClientProxy(SystemAddress systemAddress, Server* server)
			: Replicator(systemAddress, server->peerInterface()),
			  server(server)
		{
		}
		
		Server::ClientProxy::~ClientProxy() {}

		void Server::ClientProxy::onSentMarker(long id)
		{
			sendPhysicsEnabled = true;
		}

		void Server::ClientProxy::sendTop()
		{
			RakNet::BitStream bitStream;

			bitStream << (unsigned char)'K';

			std::vector<Instance*>::iterator end = replicationContainers.end();

			for (std::vector<Instance*>::iterator iter = replicationContainers.begin(); iter != end; iter++)
			{
				RBXASSERT(*iter != NULL);

				serializeId(bitStream, *iter);
				ServiceProvider::create<IdManager>(this)->addInstance(*iter);
			}

			peer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, remotePlayerId, false);
		}
	}
}
