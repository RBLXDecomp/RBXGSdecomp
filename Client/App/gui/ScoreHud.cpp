#include "gui/ScoreHud.h"
#include "Network/Players.h"
#include "v8datamodel/Teams.h"
#include "v8datamodel/Team.h"
#include "v8datamodel/Value.h"
#include "util/Hash.h"
#include "util/Color.h"
#include <vector>

namespace RBX
{	
	class BoardData
	{
	public:
		std::vector<std::vector<std::string>> data;
		std::vector<std::string> fields;

	public:
		BoardData()
		{
		}

		void SetSize(int dataSize, int fieldSize)
		{
			data.resize(dataSize);
			for (int i = 0; i < dataSize; ++i)
				data[i].resize(fieldSize);

			fields.resize(fieldSize);
		}

		void AddField(std::string fieldName, int position)
		{
			fields[position] = fieldName;
		}
	};

	class LeaderboardData
	{
	public:
		std::vector<std::string> statNamesList;
		std::map<Team*, std::vector<std::string>> playersByTeamList;
		std::map<std::string, std::map<std::string, int>> statMap;
		std::map<Team*, std::map<std::string, int>> teamTotals;
	private:
		Instance* context;

	public:
		LeaderboardData(Instance* context)
			: context(context)
		{
		}

		void CompileData()
		{
			Network::Players* players = ServiceProvider::find<Network::Players>(context);
			RBXASSERT(players);

			Teams* teams = ServiceProvider::find<Teams>(context);
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> teamList = teams->getTeams();

			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> playerList = players->getPlayers();
			for (size_t i = 0; i < playerList->size(); ++i)
			{
				Network::Player* player = static_cast<Network::Player*>(playerList->at(i).get());

				Team* team = teams->getTeamFromPlayer(player);
				playersByTeamList[team].push_back(player->getName());

				Instance* stats = player->findFirstChildByName("leaderstats");
				if (stats)
				{
					for (size_t j = 0; j < stats->numChildren(); ++j)
					{
						if (IntValue* intValue = dynamic_cast<IntValue*>(stats->getChild(j)))
							AddTuple(player->getName(), team, intValue->getName(), IntValue::desc_Value.getValue(intValue));
					}
				}
			}

			for (size_t i = 0; i < teams->numChildren(); ++i)
			{
				Team* team = dynamic_cast<Team*>(teams->getChild(i));
				RBXASSERT(team);

				for (size_t j = 0; j < statNamesList.size(); ++j)
				{
					teamTotals[team][statNamesList[j]] = 0;

					for (size_t k = 0; i < playersByTeamList[team].size(); ++k)
					{
						int statValue = statMap[playersByTeamList[team][k]][statNamesList[j]];
						teamTotals[team][statNamesList[j]] += statValue;
					}
				}
			}
		}

	private:
		void AddTuple(std::string playerName, Team* team, std::string statName, int statValue)
		{
			if (std::find(statNamesList.begin(), statNamesList.end(), statName) == statNamesList.end())
				statNamesList.push_back(statName);

			if (std::find(playersByTeamList[team].begin(), playersByTeamList[team].end(), playerName) == playersByTeamList[team].end())
				playersByTeamList[team].push_back(playerName);

			statMap[playerName][statName] = statValue;
		}
	};

	ScoreHud::ScoreHud(const ServiceProvider* serviceProvider)
		: runService(NULL),
		  players(NULL)
	{
		runService = serviceProvider ? serviceProvider->find<RunService>() : NULL;
		players = serviceProvider ? serviceProvider->find<Network::Players>() : NULL;

		RBXASSERT(runService);
		RBXASSERT(players);
	}

	ScoreHud::~ScoreHud()
	{
	}

	void ScoreHud::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		GuiItem::onServiceProvider(oldProvider, newProvider);

		runService = newProvider ? newProvider->find<RunService>() : NULL;
		players = newProvider ? newProvider->find<Network::Players>() : NULL;

		RBXASSERT(!newProvider || runService);
		RBXASSERT(!newProvider || players);
	}

	void ScoreHud::onEvent(const Instance* source, ChildAdded event)
	{
	}

	void ScoreHud::onEvent(const Instance* source, ChildRemoved event)
	{
	}

	void ScoreHud::BuildBoardData(BoardData& data)
	{
		if (!players)
			return;

		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> playerList = players->getPlayers();

		for (size_t i = 0; i < playerList->size(); ++i)
		{
			Instance* player = playerList->at(i).get();

			Instance* stats = player->findFirstChildByName("leaderstats");
			if (stats)
			{
				data.SetSize((int)playerList->size(), (int)stats->numChildren() + 1);
				data.AddField("Player", 0);

				for (size_t j = 0; j < stats->numChildren(); ++j)
				{
					Instance* stat = stats->getChild(j);
					data.AddField(stat->getName(), (int)j + 1);
				}

				data.data[i][0] = player->getName();

				for (size_t j = 0; j < stats->numChildren(); ++j)
				{
					IntValue* intValue = dynamic_cast<IntValue*>(stats->getChild(j));
					int statValue = IntValue::desc_Value.getValue(stats->getChild(j));
					data.data[i][j + 1] = boost::lexical_cast<std::string>(statValue);
				}
			}
			else
			{
				data.SetSize((int)playerList->size(), 1);
				data.AddField("Player List", 0);
				data.data[i][0] = player->getName();
			}
		}

		for (size_t i = 0; i < playerList->size(); ++i)
		{
			if (data.data[i][0].size() > 16)
				data.data[i][0].resize(16);
		}
	}

	ScoreHud::BoardType ScoreHud::DetermineBoardType()
	{
		BoardType board = PLAYER_LIST;

		bool hasStats = false;
		bool hasTeams = false;

		if (players)
		{
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> playerList = players->getPlayers();

			for (size_t i = 0; i < playerList->size(); ++i)
			{
				Network::Player* player = static_cast<Network::Player*>(playerList->at(i).get());

				if (player->findFirstChildByName("leaderstats"))
					hasStats = true;
				if (!player->getNeutral())
					hasTeams = true;
			}
		}

		if (hasStats)
			board = FFA;
		if (hasTeams)
			board = TEAM_LIST;
		if (hasStats && hasTeams)
			board = TEAM_DEATHMATCH;

		return board;
	}

	void ScoreHud::renderFFALeaderboard(Adorn* adorn)
	{
		bool hasStats = true;

		BoardData data;
		BuildBoardData(data);
		if (data.fields.empty())
			return;

		size_t fieldCount = data.fields.size();
		if (fieldCount == 0)
			return;

		if (fieldCount == 1)
			hasStats = false;

		float viewportWidth = adorn->getViewport().width();
		float scale = viewportWidth * (1.0f / 1024.0f);
		float textSize = scale * 14.0f;

		float leaderboardSizeX = viewportWidth * 0.33f;
		if (!hasStats)
			leaderboardSizeX *= 0.5f;
		float leaderboardSizeY = textSize * (data.data.size() + 2.5f);
		setGuiSize(G3D::Vector2(leaderboardSizeX, leaderboardSizeY));

		Rect uiRect = getMyRect();
		adorn->rect2d(uiRect.toRect2D(), translucentBackdrop());

		for (size_t i = 0; i < data.fields.size(); ++i)
		{
			G3D::Vector2 pos = uiRect.low;
			if (i != 0)
			{
				// leaderboard stat field
				float center = uiRect.center().x;
				pos.x += center + (center / data.fields.size()) * (i - 1);
			}

			adorn->drawFont2D(
				data.fields[i],
				pos,
				textSize,
				G3D::Color3::white(),
				G3D::Color4::clear(),
				Adorn::XALIGN_LEFT,
				Adorn::YALIGN_TOP,
				Adorn::PROPORTIONAL_SPACING
			);
		}

		G3D::Vector2 playerListTop = uiRect.low + G3D::Vector2(0, textSize + textSize);

		for (size_t i = 0; i < data.fields.size(); ++i)
		{
			for (size_t j = 0; j < data.data.size(); ++j)
			{
				G3D::Color3 textColor = G3D::Color3::white();
				if (i == 0)
				{
					// player name column
					textColor = Color::colorFromIndex8(Hash::hash(data.data[j][i]) % 8);
				}

				G3D::Vector2 pos = playerListTop;
				if (i != 0)
				{
					// leaderboard stats column
					float center = uiRect.center().x;
					pos.x += center + (center / data.fields.size()) * (i - 1);
				}
				pos.y += j * textSize;

				adorn->drawFont2D(
					data.data[j][i],
					pos,
					textSize,
					textColor,
					G3D::Color4::clear(),
					Adorn::XALIGN_LEFT,
					Adorn::YALIGN_TOP,
					Adorn::PROPORTIONAL_SPACING
				);
			}
		}
	}

	void ScoreHud::renderTeamDeathmatchLeaderboard(Adorn* adorn, BoardType board)
	{
		LeaderboardData data(this);
		data.CompileData();

		Teams* teams = ServiceProvider::find<Teams>(this);
		if (!teams)
			return;

		float viewportWidth = adorn->getViewport().width();
		float scale = viewportWidth * (1.0f / 1024.0f);
		float textSize = scale * 14.0f;

		if (board == TEAM_DEATHMATCH)
		{
			size_t teamCount = teams->numChildren();
			int playerCount = players->numPlayers();
			float leaderboardSizeX = scale * 320.0f;
			float leaderboardSizeY = playerCount + (teamCount + 1) * 3;
			setGuiSize(G3D::Vector2(leaderboardSizeX, leaderboardSizeY));
		}
		else
		{
			size_t teamCount = teams->numChildren();
			int playerCount = players->numPlayers();
			float leaderboardSizeX = scale * 240.0f;
			float leaderboardSizeY = playerCount + teamCount * 3 + 2;
			setGuiSize(G3D::Vector2(leaderboardSizeX, leaderboardSizeY));
		}

		Rect uiRect = getMyRect();
		adorn->rect2d(uiRect.toRect2D(), translucentBackdrop());

		if (board == TEAM_DEATHMATCH)
		{
			for (size_t i = 0; i < data.statNamesList.size() + 1; ++i)
			{
				float center = uiRect.center().x;
				G3D::Vector2 pos = uiRect.low;

				std::string fieldName = "Team";
				if (i != 0)
				{
					pos.x += center + (center / data.statNamesList.size()) * (i - 1);
					fieldName = data.statNamesList[i - 1];
				}

				adorn->drawFont2D(
					fieldName,
					pos,
					textSize,
					G3D::Color3::white(),
					G3D::Color4::clear(),
					Adorn::XALIGN_LEFT,
					Adorn::YALIGN_TOP,
					Adorn::PROPORTIONAL_SPACING
				);
			}

			uiRect.low.y += textSize;
		}

		for (size_t i = 0; i < teams->numChildren(); ++i)
		{
			Team* team = dynamic_cast<Team*>(teams->getChild(i));
			RBXASSERT(team);

			for (size_t j = 0; j < data.statNamesList.size() + 1; ++j)
			{
				float center = uiRect.center().x;
				G3D::Vector2 pos = uiRect.low;

				std::string fieldName = team->getName();
				if (j != 0)
				{
					pos.x += center + (center / data.statNamesList.size()) * (j - 1);
					int statValue = data.teamTotals[team][data.statNamesList[j - 1]];
					fieldName = boost::lexical_cast<std::string>(statValue);
				}

				adorn->drawFont2D(
					fieldName,
					pos,
					textSize,
					team->getTeamColor().color3(),
					G3D::Color4(0.0f, 0.0f, 0.0f),
					Adorn::XALIGN_LEFT,
					Adorn::YALIGN_TOP,
					Adorn::PROPORTIONAL_SPACING
				);
			}

			uiRect.low.y += textSize;

			adorn->line2d(
				G3D::Vector2(uiRect.low.x + 2.0f, uiRect.low.y + 7.0f),
				G3D::Vector2(uiRect.high.x - 2.0f, uiRect.low.y + 7.0f),
				team->getTeamColor().color3()
			);

			for (size_t j = 0; j < data.playersByTeamList[team].size(); ++j)
			{
				for (size_t k = 0; k < data.statNamesList.size() + 1; ++k)
				{
					float center = uiRect.center().x;
					G3D::Vector2 pos = uiRect.low;

					std::string fieldName = data.playersByTeamList[team][j];
					if (k != 0)
					{
						pos.x += center + (center / data.statNamesList.size()) * (k - 1);
						int statValue = data.statMap[data.playersByTeamList[team][j]][data.statNamesList[k - 1]];
						fieldName = boost::lexical_cast<std::string>(statValue);
					}
					else
					{
						pos.x += 5.0f;
					}

					adorn->drawFont2D(
						fieldName,
						pos,
						textSize,
						team->getTeamColor().color3(),
						G3D::Color4::clear(),
						Adorn::XALIGN_LEFT,
						Adorn::YALIGN_TOP,
						Adorn::PROPORTIONAL_SPACING
					);

					if (k == 0)
						pos.x -= 5.0f;
				}

				uiRect.low.y += textSize;
			}
		}
	}

	void ScoreHud::render2d(Adorn* adorn)
	{
		switch (DetermineBoardType())
		{
		case PLAYER_LIST:
		case FFA:
			renderFFALeaderboard(adorn);
			break;

		case TEAM_DEATHMATCH:
			renderTeamDeathmatchLeaderboard(adorn, TEAM_DEATHMATCH);
			break;

		case TEAM_LIST:
			renderTeamDeathmatchLeaderboard(adorn, TEAM_LIST);
			break;
		}
	}
}
