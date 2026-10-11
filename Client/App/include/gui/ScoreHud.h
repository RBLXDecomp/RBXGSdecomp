#pragma once
#include "gui/Gui.h"

namespace RBX
{
	class Team;
	class RunService;

	namespace Network
	{
		class Players;
	}

	class BoardData;
	class ScoreHud : public GuiItem,
		             public Listener<Instance, ChildAdded>,
					 public Listener<Instance, ChildRemoved>
	{
	protected:
		enum BoardType
		{
			PLAYER_LIST,
			FFA,
			TEAM_DEATHMATCH,
			TEAM_LIST
		};

	protected:
		RunService* runService;
		Network::Players* players;

	protected:
		void BuildBoardData(BoardData& data);
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		virtual void onEvent(const Instance* source, ChildRemoved event);
		virtual void onEvent(const Instance* source, ChildAdded event);
		virtual void render2d(Adorn* adorn);
		void renderTeamDeathmatchLeaderboard(Adorn* adorn, BoardType board);
		void renderFFALeaderboard(Adorn* adorn);
		BoardType DetermineBoardType();
	public:
		ScoreHud(const ServiceProvider* serviceProvider);
		virtual ~ScoreHud();
	};
}
