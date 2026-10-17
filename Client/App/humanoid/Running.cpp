#include "humanoid/Running.h"
#include "v8world/Primitive.h"
#include "humanoid/FallingDown.h"
#include "humanoid/Jumping.h"
#include "humanoid/Seated.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	const char* const sRunning = "Running";

	static const float manualTurnSpeed = 4.0f;
	static const float autoTurnSpeed = 8.0f;
	static const float kTurnP = 7500.0f;
	static const float kTurnAccelMax = 160000.0f;
	static const float kMoveP = 1250.0f;
	static const float kAltitudeP = 10000.0f;
	static const float kAltitudeD = 1250.0f;
	static const G3D::Vector3 maxMoveAccelerationGrid(1000.0f, 10000.0f, 200.0f);

	Running::Running(Humanoid* humanoid)
		: Base(humanoid, 300.0f, 50.0f),
		  jumpWarmup(-0.45f),
		  floorRotationalVelocity(0.0f),
		  desiredYRotationalVelocity(0.0f),
		  desiredAltitude(0.0f),
		  intendedYRotation(0.0f)
	{
		if (humanoid->getLeftLegPrimitive())
			humanoid->getLeftLegPrimitive()->setCanCollide(false);

		if (humanoid->getRightLegPrimitive())
			humanoid->getRightLegPrimitive()->setCanCollide(false);

		Humanoid::event_Running.fire(humanoid, 0.0f);
	}

	Running::~Running()
	{
		Humanoid::event_Running.fire(humanoid, 0.0f);

		if (humanoid->getLeftLegPrimitive())
			humanoid->getLeftLegPrimitive()->setCanCollide(true);

		if (humanoid->getRightLegPrimitive())
			humanoid->getRightLegPrimitive()->setCanCollide(true);
	}

	Humanoid::State* Running::onStep(const float stepDt, Controller& controller)
	{
		Humanoid::State* check = Balancing::onStep(stepDt, controller);
		if (check == this)
		{
			G3D::Vector3 yAxis = Balancing::getYAxis();
			float y = yAxis.y;

			if (y < 0.85f)
			{
				return new FallingDown(humanoid, 3.0f);
			}

			if (jumpWarmup >= 0.0f)
			{
				if (humanoid->getJump())
				{
					humanoid->setJump(false);
					return new Jumping(humanoid);
				}
			}

			humanoid->setJump(false);

			if (humanoid->getSit())
			{
				humanoid->setSit(false);
				return new Seated(humanoid);
			}

			if (floorPart)
				humanoid->getWorld()->ticklePrimitive(floorPart->getPrimitive());

			desiredAltitude = floorTouchInWorld.y;

			if (humanoid->getTorsoPrimitive())
			{
				desiredAltitude += humanoid->getTorsoPrimitive()->getGeometry()->getGridSize().y * 0.5f;
			}

			if (humanoid->getLeftLegPrimitive())
			{
				desiredAltitude += humanoid->getLeftLegPrimitive()->getGeometry()->getGridSize().y;
			}
			else if (humanoid->getRightLegPrimitive())
			{
				desiredAltitude += humanoid->getRightLegPrimitive()->getGeometry()->getGridSize().y;
			}

			Velocity vel = (!floorPart) ? Velocity::zero() : floorPart->getPrimitive()->getBody()->getPV().velocityAtPoint(floorTouchInWorld);

			G3D::Vector3 floorVel = vel.linear;
			G3D::Vector3 floorRotVel = vel.rotational;

			floorVelocity = floorVel;
			floorRotationalVelocity = floorRotVel.y;
			updateGoals(controller);
			return this;
		}
		else
		{
			return check;
		}
	}
}
