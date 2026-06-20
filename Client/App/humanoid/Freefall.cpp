#include "humanoid/Freefall.h"
#include "humanoid/Running.h"

namespace RBX
{
	Freefall::Freefall(Humanoid* humanoid)
		: Base(humanoid)
	{
	}

	Humanoid::State* Freefall::onStep(const float stepDt, Controller& controller)
	{
		Humanoid::State* newState = Flying::onStep(stepDt, controller);

		if (newState == this)
		{
			if (floorPart)
				return new Running(humanoid);
			else
				return this;
		}
		else
		{
			return newState;
		}
	}
}
