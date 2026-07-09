#include <RakNetTypes.h>
#include <RakPeer.h>
#include "API.h"
#include "Client.h"
#include "util/standardout.h"

static RBX::Reflection::BoundFuncDesc<RBX::Network::Client, void(std::string, int, int, int), 4> f_connect(&RBX::Network::Client::connect, "Connect", "server", "serverPort", "clientPort", "threadSleepTime", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Client, void(int), 1> f_disconnect(&RBX::Network::Client::disconnect, "Disconnect", "blockDuration", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

const char* Exposer::IDTOString(int id)
{
	Exposer exposer;
	return exposer.PacketLogger::IDTOString((unsigned char)id);
}

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
			Listener<ServiceProvider, Closing>* oldListener = this;

			if (oldProvider)
			{
				oldProvider->Notifier<ServiceProvider, Closing>::removeListener(oldListener);
			}

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

			Listener<ServiceProvider, Closing>* newListener = this;

			if (newProvider)
			{
				newProvider->Notifier<ServiceProvider, Closing>::addListener(newListener);
			}
		}
	}
}
