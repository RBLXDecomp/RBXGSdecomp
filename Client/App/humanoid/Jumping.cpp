#include "humanoid/Jumping.h"
#include "humanoid/Freefall.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/World.h"

namespace RBX
{
	const char* const sJumping = "Jumping";

	static const float kJumpP = 5000.0f;
	static const float jumpVelocityGrid = 50.0f;

	Jumping::Jumping(Humanoid* humanoid)
		: Base(humanoid),
		  timeout(0.5f),
		  done(false)
	{
		Humanoid::event_Jumping.fire(humanoid, true);
	}

	Jumping::~Jumping()
	{
		Humanoid::event_Jumping.fire(humanoid, false);
	}

	Humanoid::State* Jumping::onStep(const float stepDt, Controller& controller)
	{
		if (done)
		{
			return new Freefall(humanoid);
		}
		else
		{
			Humanoid::State* newState = Flying::onStep(stepDt, controller);

			if (newState == this)
			{
				if (floorPart)
				{
					humanoid->getWorld()->ticklePrimitive(floorPart->getPrimitive());
					return this;
				}
				else
				{
					return new Freefall(humanoid);
				}
			}
			else
			{
				return newState;
			}
		}
	}
}
