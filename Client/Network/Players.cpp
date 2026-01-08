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

		Player* Players::findLocalPlayer(const Instance* context)
		{
			Players* players = ServiceProvider::find<Players>(context);
			return players ? players->getLocalPlayer() : NULL;
		}

		ModelInstance* Players::findLocalCharacter(const Instance* context)
		{
			Player* player = Players::findLocalPlayer(context);
			return player ? player->getCharacter() : NULL;
		}

		void Players::setAbuseReportUrl(std::string value)
		{
			if (value.empty())
				abuseReporter.reset(NULL);
			else
				abuseReporter.reset(new AbuseReporter(value));
		}

		void Players::reportAbuse(boost::shared_ptr<Instance> player, std::string comment)
		{
			reportAbuse(fastDynamicCast<Player>(player.get()), comment);
		}

		Player* Players::getPlayerFromCharacter(Instance* character)
		{
			Players* players = ServiceProvider::find<Players>(character);
			if (players)
			{
				boost::shared_ptr<Instance> found = players->playerFromCharacter(shared_from(character));
				return static_cast<Player*>(found.get());
			}
			else
			{
				return NULL;
			}
		}
	}
}
