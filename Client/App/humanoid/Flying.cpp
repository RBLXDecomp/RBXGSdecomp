#include "humanoid/Flying.h"
#include "humanoid/FallingDown.h"
#include "v8world/Controller.h"

namespace RBX
{
	const char* const sFlying = "Flying";

	static const float turnSpeed = 6.0f;
	static const float kTurnP = 7500.0f;
	static const float kTurnAccelMax = 120000.0f;

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

	void Flying::onComputeForce(const float dt)
	{
		Balancing::onComputeForce(dt);

		Body* rootBody = humanoid->getRootBody();
		RBXASSERT(rootBody);
		
		float y = rootBody->getBranchIBodyV3().y;
		float var1 = (desiredYRotationalVelocity - rootBody->getVelocity().rotational.y) * y;
		var1 *= kTurnP;

		G3D::Vector3 oldTorque = rootBody->getBranchTorque();

		float desiredTorqueY  = var1 - oldTorque.y;
		float var2 = kTurnAccelMax * rootBody->getBranchIBodyV3().y;
		desiredTorqueY = G3D::clamp(desiredTorqueY, -var2, var2);

		rootBody->accumulateTorque(G3D::Vector3(0.0f, desiredTorqueY, 0.0f));
	}
}
