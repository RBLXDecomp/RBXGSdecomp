#pragma once
#include "humanoid/Balancing.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sGettingUp;

	class GettingUp : public Named<Balancing, &sGettingUp>
	{
	protected:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);
		virtual bool canSleep() const
		{
			return false;
		}
	public:
		GettingUp(Humanoid* humanoid);
		virtual ~GettingUp();
	};
}
