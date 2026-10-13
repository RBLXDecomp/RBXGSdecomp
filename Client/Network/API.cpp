#include "API.h"
#include "Client.h"
#include "Server.h"
#include "IdManager.h"
#include "NetworkSettings.h"
#include "Network/Players.h"

namespace RBX
{
	namespace Network
	{
		std::string API::version;

		void API::init(const char* version)
		{
			API::version = version;

			Client::classDescriptor();
			Server::classDescriptor();
			Player::classDescriptor();
			Players::classDescriptor();
			IdManager::classDescriptor();
			GlobalSettings::classDescriptor();
			NetworkSettings::classDescriptor();

			NetworkSettings::singleton();
		}
	}
}
