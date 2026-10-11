#pragma once
#include "gui/Gui.h"
#include "gui/ProfanityFilter.h"
#include "Network/Players.h"

namespace RBX
{
	class ChatLine
	{
	public:
		G3D::Color3 userColor;
		std::string user;
		std::string message;
		float dieTime;
		bool profane;

	public:
		ChatLine(Network::Player* player, const std::string& message, float dieTime);
		ChatLine(const std::string&, const std::string&, float);
	};

	class ChatOutput : public GuiItem,
					   public Listener<Network::Players, Network::ChatMessage>,
					   public Listener<RunService, Heartbeat>
	{
	private:
		RunService* runService;
		Network::Players* players;
		ProfanityFilter filter;
		std::deque<ChatLine*> fifo;
		float time;

	private:
		void removeOldest();
		void frontIsOld();
		bool ParseCommand(Network::ChatMessage message);
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		virtual void onEvent(const Network::Players* source, Network::ChatMessage event);
		virtual void onEvent(const RunService* source, Heartbeat event);
		virtual void render2d(Adorn* adorn);
	public:
		ChatOutput();
		virtual ~ChatOutput();
	};
}
