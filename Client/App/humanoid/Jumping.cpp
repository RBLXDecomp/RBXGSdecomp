#include "humanoid/Jumping.h"
#include "humanoid/Freefall.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/World.h"

namespace RBX
{
	const char* const sJumping = "Jumping";

	const float Jumping::kJumpP = 5000.0f;
	const float Jumping::jumpVelocityGrid = 50.0f;

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

	// 98% match
	void Jumping::onComputeForce(const float dt)
	{
		Flying::onComputeForce(dt);

		timeout -= dt;
		if (done)
			return;

		Body* rootBody = humanoid->getRootBody();

		const float yAccelDesired = rootBody ? (jumpVelocityGrid - rootBody->getVelocity().linear.y) * kJumpP : 0.0f;

		if (yAccelDesired <= 0.0f)
		{
			done = true;
			return;
		}

		if (floorPart)
		{
			Body* floorRootBody = floorPart->getPrimitive()->getBody()->getRoot();

			const float newForceY = G3D::min(rootBody->getBranchMass(), floorRootBody->getBranchMass()) * yAccelDesired;
			const G3D::Vector3& oldForce = rootBody->getBranchForce();

			if (newForceY > oldForce.y)
			{
				const float diff = newForceY - oldForce.y;
				rootBody->accumulateForceAtBranchCofm(G3D::Vector3(0.0f, diff, 0.0f));
				floorRootBody->accumulateForce(G3D::Vector3(0.0f, -diff, 0.0f), floorTouchInWorld);
			}
		}
	}
}
