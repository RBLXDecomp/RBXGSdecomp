#include "v8datamodel/Teams.h"
#include "v8datamodel/ModelInstance.h"
#include "Network/Players.h"

namespace RBX
{
	const char* sTeams = "Teams";

	Teams::Teams()
	{
		setName("Teams");
	}

	Team::~Team()
	{
	}

	int Teams::getNumPlayersInTeam(BrickColor brickColor)
	{
		const ServiceProvider* provider = ServiceProvider::findServiceProvider(this);
		Network::Players* players = provider->find<Network::Players>();
		RBXASSERT(players);

		int numOfPlayers = 0;
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
		const ServiceProvider* provider = ServiceProvider::findServiceProvider(this);
		Network::Players* players = provider->find<Network::Players>();
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
		BrickColor currentTeam(BrickColor::lego_28);
		bool foundTeam = false;

		for (size_t i = 0; i < numChildren(); i++)
		{
			Team* team = fastDynamicCast<Team>(getChild(i));
			if (team)
			{
				if (team->getAutoAssignable() == true)
				{
					if (getNumPlayersInTeam(team->getTeamColor()) < currentTeam.number)
					{
						currentTeam = team->getTeamColor();
						foundTeam = true;
					}
				}
			}
		}
		
		if (foundTeam)
		{
			player->setTeamColor(currentTeam);
			player->setNeutral(false);
		}
	}
}