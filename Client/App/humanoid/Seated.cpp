#include "humanoid/Seated.h"
#include "humanoid/Jumping.h"

namespace RBX
{
	Seated::Seated(Humanoid* humanoid)
		: Base(humanoid)
	{
		Humanoid::event_Seated.fire(humanoid, true);
	}

	Seated::~Seated()
	{
		Humanoid::event_Seated.fire(humanoid, false);
	}

	Humanoid::State* Seated::onStep(const float stepDt, Controller& controller)
	{
		if (humanoid->getJump())
		{
			humanoid->setJump(false);
			return new Jumping(humanoid);
		}
		else
		{
			return this;
		}
	}
}
