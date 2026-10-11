#include "gui/ChatOutput.h"
#include "v8datamodel/Teams.h"
#include "v8datamodel/GameSettings.h"
#include "v8datamodel/SafeChat.h"
#include "Network/Players.h"
#include "util/Hash.h"
#include "util/Color.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

namespace RBX
{
	ChatLine::ChatLine(Network::Player* player, const std::string& message, float dieTime)
		: message(message),
		  dieTime(dieTime),
		  profane(false)
	{
		RBXASSERT(player);
		user = player->getName();

		Teams* teams = ServiceProvider::find<Teams>(player);
		if (teams && teams->isTeamGame())
		{
			if (!player->getNeutral())
			{
				if (Team* team = teams->getTeamFromPlayer(player))
				{
					userColor = team->getTeamColor().color3();
				}
				else
				{
					userColor = G3D::Color3::black();
				}
			}
			else
			{
				userColor = G3D::Color3::white();
			}
		}
		else
		{
			userColor = Color::colorFromIndex8(Hash::hash(player->getName()) % 8);
		}
	}

	ChatOutput::ChatOutput()
		: runService(NULL),
		  players(NULL),
		  time(0.0f)
	{
	}

	ChatOutput::~ChatOutput()
	{
		while (!fifo.empty())
			removeOldest();
	}

	void ChatOutput::removeOldest()
	{
		ChatLine* oldest = fifo.front();
		fifo.pop_front();
		delete oldest;
	}

	void ChatOutput::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Notifier<RunService, Heartbeat>::disconnect(runService, this);
		Notifier<Network::Players, Network::ChatMessage>::disconnect(players, this);

		GuiItem::onServiceProvider(oldProvider, newProvider);

		runService = newProvider ? newProvider->create<RunService>() : NULL;
		players = newProvider ? newProvider->create<Network::Players>() : NULL;
		Notifier<RunService, Heartbeat>::connect(runService, this);
		Notifier<Network::Players, Network::ChatMessage>::connect(players, this);
	}

	bool ChatOutput::ParseCommand(Network::ChatMessage message)
	{
		std::vector<std::string> code;

		typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
		boost::char_separator<char> sep(" -_/,");
		tokenizer tok(message.message, sep);
		for (tokenizer::const_iterator it = tok.begin(); it != tok.end(); ++it)
		{
			code.push_back(*it);
		}

		if (code.empty())
			return false;

		boost::to_lower(code[0]);
		if (code[0] == "sc")
		{
			std::string safeChatMessage = SafeChat::singleton().getMessage(code);
			fifo.push_back(new ChatLine(message.source.get(), safeChatMessage, time + 60.0f));
			return true;
		}

		return false;
	}

	void ChatOutput::onEvent(const Network::Players* source, Network::ChatMessage event)
	{
		while (fifo.size() > (size_t)GameSettings::singleton().chatScrollLength)
			removeOldest();

		if (event.message.size() > 0 && event.message[0] == '/' && ParseCommand(event) == true)
			return;

		Network::Player* localPlayer = players->getLocalPlayer();
		if (!localPlayer || !localPlayer->getSuperSafeChat())
		{
			bool profane = filter.ContainsProfanity(event.message);
			if (!localPlayer || event.source.get() == localPlayer || !profane)
			{
				ChatLine* chatLine = new ChatLine(event.source.get(), event.message, time + 60.0f);
				chatLine->profane = false;
				fifo.push_back(chatLine);
			}
		}
	}

	void ChatOutput::onEvent(const RunService* source, Heartbeat event)
	{
		time += event.step;

		while (fifo.size() > 0)
		{
			ChatLine* oldest = fifo.front();
			if (oldest->dieTime >= time)
				break;
			else
				removeOldest();
		}
	}

	void ChatOutput::render2d(Adorn* adorn)
	{
		const int fontSize = 12;

		Rect uiRect = getMyRect();

		int yOffset = fontSize * (int)fifo.size();
		G3D::Vector2 topPos = uiRect.low - G3D::Vector2(0, yOffset);

		for (size_t i = 0; i < fifo.size(); ++i)
		{
			std::string username = fifo[i]->user + "; ";

			G3D::Vector2 usernameBounds = adorn->get2DStringBounds(username, fontSize, Adorn::PROPORTIONAL_SPACING);
			G3D::Vector2 messageBounds = adorn->get2DStringBounds(fifo[i]->message, fontSize, Adorn::PROPORTIONAL_SPACING);
			float usernameXOffset = G3D::max(20.0f, usernameBounds.x + 4.0f);

			Rect backgroundRect(
				G3D::Vector2(topPos.x, topPos.y) + G3D::Vector2(-2.0f, 1.0f),
				G3D::Vector2(topPos.x + messageBounds.x + usernameXOffset, topPos.y + usernameBounds.y) - G3D::Vector2(-2.0f, 1.0f)
			);
			adorn->rect2d(backgroundRect.toRect2D(), translucentBackdrop());

			G3D::Color3 messageColor = G3D::Color3::white();
			if (fifo[i]->profane)
				messageColor = G3D::Color3::red();

			adorn->drawFont2D(
				username,
				topPos,
				fontSize,
				fifo[i]->userColor,
				G3D::Color4::clear(),
				Adorn::XALIGN_LEFT,
				Adorn::YALIGN_TOP,
				Adorn::PROPORTIONAL_SPACING
			);

			adorn->drawFont2D(
				fifo[i]->message,
				topPos + G3D::Vector2(usernameXOffset, 0.0f),
				fontSize,
				messageColor,
				G3D::Color4::clear(),
				Adorn::XALIGN_LEFT,
				Adorn::YALIGN_TOP,
				Adorn::PROPORTIONAL_SPACING
			);

			topPos.y += fontSize + 2;
		}
	}
}
