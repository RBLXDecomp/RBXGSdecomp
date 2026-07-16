#pragma once
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	class Weld;
	class Humanoid;

	extern const char* sSeat;

	class Seat : public DescribedCreatable<Seat, PartInstance, &sSeat>
	{
	private:
		boost::signals::scoped_connection seatTouched;
		boost::signals::scoped_connection humanoidJumped;
		boost::shared_ptr<Weld> weld;
		double sleepTime;

	private:
		void onEvent_seatTouched(boost::shared_ptr<Instance> other);
		void onEvent_humanoidJumped(bool active);
		void seatCharacter(Humanoid* h);
		void unseatCharacter();
		bool charSeated();
	public:
		Seat();
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
	};
}
