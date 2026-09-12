#include <RakNetTypes.h>
#include <RakPeer.h>
#include <MessageIdentifiers.h>
#include "API.h"
#include "Client.h"
#include "util/standardout.h"

static RBX::Reflection::BoundFuncDesc<RBX::Network::Client, void(std::string, int, int, int), 4> f_connect(&RBX::Network::Client::connect, "Connect", "server", "serverPort", "clientPort", "threadSleepTime", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Client, void(int), 1> f_disconnect(&RBX::Network::Client::disconnect, "Disconnect", "blockDuration", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

class Exposer : protected PacketLogger
{
public:
	static const char* IDTOString(int id)
	{
		Exposer exposer;
		return exposer.PacketLogger::IDTOString((unsigned char)id);
	}
};

namespace RBX
{
	namespace Network
	{
		Client::Client()
		{
			setName("NetworkClient");
			updateLogger();
		}

		Client::~Client()
		{
			rakPeer->CloseConnection(serverId, true, 0);
		}

		void Client::disconnect(int blockDuration)
		{
			removeAllChildren();
			rakPeer->CloseConnection(serverId, true, 0);
			rakPeer->Shutdown(blockDuration, 0);
		}

		void Client::onEvent(const ServiceProvider* source, Closing event)
		{
			disconnect(3000);
		}

		void Client::connect(std::string server, int serverPort, int clientPort, int threadSleepTime)
		{
			SocketDescriptor d(clientPort, "");
			if (!rakPeer->Startup(1, threadSleepTime, &d, 1))
				throw std::runtime_error("Failed to start network client");

			if (!rakPeer->Connect(server.c_str(), serverPort, API::version.c_str(), (int)API::version.size()))
				throw std::runtime_error("Failed to connect to server");

			StandardOut::singleton()->print(MESSAGE_INFO, "Connecting to %s:%d", server.c_str(), serverPort);

			updateNetworkSimulator();
		}

		void Client::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
		{
			Notifier<ServiceProvider, Closing>::disconnect(oldProvider, this);

			if (oldProvider)
			{
				RunService* runService = oldProvider->find<RunService>();
				if (runService)
					runService->runDisabled = false;

				disconnect(3000);

				Players* p = oldProvider->find<Players>();
				p->setConnection(NULL);
			}

			Instance::onServiceProvider(oldProvider, newProvider);

			if (newProvider)
			{
				Players* p = newProvider->create<Players>();
				p->setConnection(rakPeer.get());

				RunService* runService = newProvider->find<RunService>();
				if (runService)
					runService->runDisabled = true;
			}

			Notifier<ServiceProvider, Closing>::connect(newProvider, this);
		}

		bool Client::clientIsPresent(const Instance* context, bool testInDatamodel)
		{
			const ServiceProvider* sp = ServiceProvider::findServiceProvider(context);
			RBXASSERT(!testInDatamodel || sp);

			return ServiceProvider::find<Client>(sp) != NULL;
		}

		PluginReceiveResult Client::OnReceive(RakPeerInterface* peer, Packet* packet)
		{
			PluginReceiveResult result = PluginInterface::OnReceive(peer, packet);
			if (result != RR_CONTINUE_PROCESSING)
				return result;

			switch(packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				try
				{
					StandardOut::singleton()->print(MESSAGE_INFO, "Connection accepted from %s\n", packet->systemAddress.ToString());
					serverId = packet->systemAddress;
					
					boost::shared_ptr<Replicator> proxy = Creatable::create<Replicator>(packet->systemAddress, this->rakPeer.get());
					proxy->setParent(this);
					
					event_ConnectionAccepted.fire(this, packet->systemAddress.ToString(), proxy);
				}
				catch (std::exception& e)
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Client::OnReceive packet %d: %s", packet->data[0], e.what());
				}
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			case ID_CONNECTION_BANNED:
				try
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Failed to connect to %s. %s\n", packet->systemAddress.ToString(), Exposer::IDTOString(packet->data[0]));
					event_ConnectionFailed.fire(this, packet->systemAddress.ToString(), packet->data[0]);
				}
				catch (std::exception& e)
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Client::OnReceive packet %d: %s", packet->data[0], e.what());
				}
				break;
			
			case ID_INVALID_PASSWORD:
				try
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Invalid version from %s", packet->systemAddress.ToString());
					event_ConnectionFailed.fire(this, packet->systemAddress.ToString(), packet->data[0]);
					event_ConnectionRejected.fire(this, packet->systemAddress.ToString());
				}
				catch (std::exception& e)
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Client::OnReceive packet %d: %s", packet->data[0], e.what());
				}
				break;

			case ID_DISCONNECTION_NOTIFICATION:
			case ID_CONNECTION_LOST:
				RBXASSERT(packet->systemAddress == serverId);
				serverId = UNASSIGNED_SYSTEM_ADDRESS;
				break;
			}

			return RR_CONTINUE_PROCESSING;
		}
	}
}
