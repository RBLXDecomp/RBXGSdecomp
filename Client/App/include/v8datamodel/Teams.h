#pragma once
#include "v8tree/Service.h"
#include "v8datamodel/BrickColor.h"
#include "v8datamodel/Team.h"
#include "util/Utilities.h"
#include <vector>

namespace RBX
{
	namespace Network
	{
		class Player;
	}
	class Humanoid;

	extern const char* sTeams;
	class Teams : public DescribedCreatable<Teams, Instance, &sTeams>, public Service
	{
	private:
		CopyOnWrite<std::vector<boost::shared_ptr<Instance>>> teams;

	public:
		//Teams(const Teams&);
		Teams();
		virtual ~Teams();
	public:
		void assignNewPlayerToTeam(Network::Player*);
		int getNumPlayersInTeam(BrickColor);
		bool teamExists(BrickColor);
		bool isTeamGame();
		void rebalanceTeams();
		BrickColor getUnusedTeamColor();
		Team* getTeamFromTeamColor(BrickColor);
		Team* getTeamFromPlayer(Network::Player*);
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> getTeams();
		G3D::Color3 getTeamColorForHumanoid(Humanoid*);
	protected:
		virtual void onChildAdded(Instance*);
		virtual void onChildRemoving(Instance*);
	public:
		//Teams& operator=(const Teams&);
	};
}
