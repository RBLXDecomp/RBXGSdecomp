#pragma once
#include "humanoid/Flying.h"

namespace RBX
{
	extern const char* const sFreefall;

	class Freefall : public Named<Flying, &sFreefall>
	{
	private:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);

		virtual bool canSleep() const
		{
			return false;
		}

	public:
		Freefall(Humanoid* humanoid);
	};
}
