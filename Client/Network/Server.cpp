#include <RakPeer.h>
#include "Server.h"
#include "IdManager.h"
#include "API.h"
#include "util/standardout.h"

static bool isReplicator(boost::shared_ptr<RBX::Instance> instance)
{
	return RBX::Instance::fastDynamicCast<const RBX::Network::Replicator>(instance.get()) != NULL;
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
			if (&(*getChildren()))
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

			bitStream << 'K';

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
