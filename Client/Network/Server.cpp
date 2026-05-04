#include <RakPeer.h>
#include "Server.h"
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
	}
}
