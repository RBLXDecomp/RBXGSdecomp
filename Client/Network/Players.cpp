#include "Players.h"

namespace RBX
{
	namespace Network
	{
		bool Players::clientIsPresent(const Instance* context, bool testInDatamodel)
		{
			return Client::clientIsPresent(context, testInDatamodel);
		}

		bool Players::askAddChild(const Instance* instance) const
		{
			return fastDynamicCast<const Player>(instance) != NULL;
		}

		void Players::setConnection(RakPeerInterface* peer)
		{
			if (this->peer)
				this->peer->DetachPlugin(plugin.get());

			this->peer = peer;

			if (peer)
				peer->AttachPlugin(plugin.get());
		}
	}
}