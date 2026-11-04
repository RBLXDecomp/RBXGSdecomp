#pragma once
#include "v8datamodel/Tool.h"
#include "v8datamodel/BrickColor.h"

namespace RBX
{
	class TimerService;
		
	namespace Network
	{
		class Player;
	}

	extern const char* const sFlag;

	class Flag : public DescribedCreatable<Flag, Tool, &sFlag>
	{
		enum FlagState
		{
			NOTHING,
			LOOSE,
			IN_STAND,
			EQUIPPED
		};

	private:
		boost::shared_ptr<Instance> evilClone;
		FlagState state;
		boost::signals::scoped_connection flagTouched;
		TimerService* timer;
	public:
		BrickColor teamColor;

	private:
		void onEvent_flagTouched(boost::shared_ptr<Instance> other);
		void doUglyPeriodicCloneHack();
	protected:
		virtual bool canUnequip()
		{
			return false;
		}

		virtual bool canBePickedUpByPlayer(Network::Player* p);
	public:
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor color);
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		Flag();
		virtual ~Flag();
		virtual void onChildAdded(Instance* instance);
	};
}
