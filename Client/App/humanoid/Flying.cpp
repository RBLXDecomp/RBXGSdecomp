#include "humanoid/Flying.h"
#include "humanoid/FallingDown.h"
#include "v8world/Controller.h"

namespace RBX
{
	Flying::Flying(Humanoid* humanoid)
		: Base(humanoid, 300.0f, 50.0f),
		  desiredYRotationalVelocity(0.0f)
	{
	}

	Humanoid::State* Flying::onStep(const float stepDt, Controller& controller)
	{
		Humanoid::State* newState = Balancing::onStep(stepDt, controller);
		
		if (newState == this)
		{
			G3D::Vector3 yAxis = getYAxis();
			if (yAxis.y < 0.85f)
			{
				return new FallingDown(humanoid, 3.0f);
			}
			else
			{
				desiredYRotationalVelocity = controller.getValue(Controller::RIGHT_LEFT_INPUT) * 6.0f;
				return this;
			}
		}
		else
		{
			return newState;
		}
	}
}
