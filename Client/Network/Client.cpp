#include "util/standardout.h"
#include "API.h"
#include "Client.h"
#include <RakNetTypes.h>

const char* Exposer::IDTOString(int id)
{
	Exposer exposer;
	return exposer.PacketLogger::IDTOString((unsigned char)id);
}

namespace RBX
{
	namespace Network
	{
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
	}
}
