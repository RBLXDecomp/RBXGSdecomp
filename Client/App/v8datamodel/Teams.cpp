#include "v8datamodel/Teams.h"
#include "v8datamodel/ModelInstance.h"
#include "Network/Players.h"

namespace RBX
{
	const char* sTeams = "Teams";

	Teams::Teams()
		: teams(std::vector<boost::shared_ptr<Instance>>())
	{
		setName("Teams");
	}

	Teams::~Teams()
	{
	}

	int Teams::getNumPlayersInTeam(BrickColor brickColor)
	{
		int numOfPlayers = 0;

		Network::Players* players = ServiceProvider::find<Network::Players>(this);
		RBXASSERT(players);

		for (size_t i = 0; i < players->numChildren(); i++)
		{
			Network::Player* player = fastDynamicCast<Network::Player>(players->getChild(i));
			if (player)
			{
				if (!player->getNeutral() && player->getTeamColor() == brickColor)
					numOfPlayers++;
			}
		}
		
		return numOfPlayers;
	}
	
	bool Teams::teamExists(BrickColor brickColor)
	{
		return getTeamFromTeamColor(brickColor) != NULL;
	}

	Team* Teams::getTeamFromTeamColor(BrickColor brickColor)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			Team* team = fastDynamicCast<Team>(getChild(i));
			if (team && team->getTeamColor() == brickColor)
				return team;
		}
		
		return NULL;
	}

	G3D::Color3 Teams::getTeamColorForHumanoid(Humanoid* humanoid)
	{
		Network::Players* players = ServiceProvider::find<Network::Players>(this);
		RBXASSERT(players);

		for (size_t i = 0; i < players->numChildren(); i++)
		{
			Network::Player* player = fastDynamicCast<Network::Player>(players->getChild(i));
			if (player)
			{
				if (!player->getNeutral() && player->getCharacter())
				{
					ModelInstance* character = player->getCharacter();
					if (character->findFirstChildOfType<Humanoid>() == humanoid)
					{
						return player->getTeamColor().color3();
					}
				}
			}
		}

		return G3D::Color3::white();
	}

	void Teams::assignNewPlayerToTeam(Network::Player* player)
	{
		BrickColor best = BrickColor::lego_28;
		int best_count = 10000;
		bool found = false;

		for (size_t i = 0; i < numChildren(); i++)
		{
			Team* team = fastDynamicCast<Team>(getChild(i));
			if (team && team->getAutoAssignable() == true)
			{
				int count = getNumPlayersInTeam(team->getTeamColor());

				if (count < best_count)
				{
					best = team->getTeamColor();
					best_count = count;
					found = true;
				}
			}
		}
		
		if (found)
		{
			player->setTeamColor(best);
			player->setNeutral(false);
		}
	}

	void Teams::onChildAdded(Instance* child)
	{
		if (fastDynamicCast<Team>(child))
		{
			boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>>& myTeams = teams.write();
			myTeams->push_back(shared_from(child));
		}
	}

	void Teams::onChildRemoving(Instance* child)
	{
		if (fastDynamicCast<Team>(child))
		{
			boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>>& myTeams = teams.write();
			myTeams->erase(std::find(myTeams->begin(), myTeams->end(), shared_from(child)));
		}
	}
}
