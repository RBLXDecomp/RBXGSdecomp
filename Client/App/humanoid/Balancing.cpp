#include "humanoid/Balancing.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/Primitive.h"
#include "v8world/ContactManager.h"

namespace RBX
{
	Balancing::Balancing(Humanoid* humanoid, const float kP, const float kD)
		: Humanoid::State(humanoid),
		  kP(kP),
		  kD(kD)
	{
	}

	PartInstance* Balancing::tryFloor(const G3D::Ray& ray, float maxDistance)
	{
		float distance;

		Primitive* hit = humanoid->getWorld()->getContactManager().getHitLegacy(ray, NULL, this, floorTouchInWorld, distance, maxDistance);
		if (hit)
			return PartInstance::fromPrimitive(hit);
		else
			return NULL;
	}

	Humanoid::State* Balancing::onStep(const float stepDt, Controller& controller)
	{
		if (humanoid->getTorsoBody())
		{
			floorPart = shared_from(findFloor(floorPart != NULL));
			return this;
		}
		else
		{
			return NULL;
		}
	}

	G3D::Vector3 Balancing::getYAxis() const
	{
		Body* torsoBody = humanoid->getTorsoBody();

		if (torsoBody)
		{
			G3D::Vector3 yAxis = torsoBody->getCoordinateFrame().vectorToObjectSpace(G3D::Vector3::unitY());
			return yAxis;
		}
		else
		{
			RBXASSERT(false);
			return G3D::Vector3::unitY();
		}
	}

	HitTestFilter::Result Balancing::filterResult(const Primitive* testMe) const
	{
		RBXASSERT(testMe);
		Primitive* torsoPrim = humanoid->getTorsoPrimitive();
		HitTestFilter::Result result = INCLUDE_PRIM;

		if (testMe->getDragging() == false && testMe->getCanCollide() != false)
		{
			if (torsoPrim)
			{
				if (torsoPrim->getAssembly() == testMe->getAssembly())
				{
					result = IGNORE_PRIM;
				}
			}
		}
		else
		{
			result = IGNORE_PRIM;
		}

		return result;
	}

	// 99.91% match
	void Balancing::onComputeForce(const float dt)
	{
		Body* torsoBody = humanoid->getTorsoBody();
		if (!torsoBody)
			return;

		Body* rootBody = humanoid->getRootBody();
		if (!rootBody)
			return;

		G3D::Vector3 yAxisWorld = torsoBody->getCoordinateFrame().upVector();

		G3D::Vector3 tiltWorld = G3D::Vector3::unitY().cross(yAxisWorld);
		G3D::Vector3 angVelWorld = torsoBody->getVelocity().rotational;
		G3D::Vector3 oldTorqueWorld = rootBody->getBranchTorque();

		const G3D::CoordinateFrame& cfRoot = rootBody->getCoordinateFrame();

		G3D::Vector3 tiltRoot = cfRoot.vectorToObjectSpace(tiltWorld);
		G3D::Vector3 angVelRoot = cfRoot.vectorToObjectSpace(angVelWorld);
		G3D::Vector3 oldTorqueRoot = cfRoot.vectorToObjectSpace(oldTorqueWorld);

		RBXASSERT(!Math::isNanInfDenormVector3(angVelRoot));

		G3D::Vector3 addedTorque = -kP * rootBody->getBranchIBody() * tiltRoot;
		G3D::Vector3 torqueRoot = oldTorqueRoot + addedTorque;

		G3D::Vector3 branchIBody = Math::toDiagonal(rootBody->getBranchIBody()); // could be branchIBodyV3, though it aint inlining here

		for (int i = 0; i < 3; i += 2)
		{
			if (fabs(addedTorque[i] - oldTorqueRoot[i]) < branchIBody[i] * 4000.0f)
			{
				torqueRoot[i] = addedTorque[i];
			}
		}

		G3D::Vector3 desiredTorque = cfRoot.vectorToWorldSpace(torqueRoot - branchIBody * kD * angVelRoot) - oldTorqueWorld;
		rootBody->accumulateTorque(desiredTorque);
	}

	// 99.79% match
	PartInstance* Balancing::findFloor(bool existingFloor)
	{
		Body* torsoBody = humanoid->getTorsoBody();
		if (torsoBody)
		{
			const G3D::CoordinateFrame& cframe = humanoid->getTorsoPrimitive()->getCoordinateFrame();
			G3D::Vector3 torsoSize = humanoid->getTorsoPrimitive()->getGridSize() * 0.5f;

			float maxDistance = 1.0f;
			const float hysteresis = existingFloor ? 1.5f : 1.1f;

			if (humanoid->getLeftLegPrimitive())
			{
				maxDistance = humanoid->getLeftLegPrimitive()->getGridSize().y * hysteresis + 1.0f;
			}
			else if (humanoid->getRightLegPrimitive())
			{
				maxDistance = humanoid->getRightLegPrimitive()->getGridSize().y * hysteresis + 1.0f;
			}

			G3D::Vector3 offset(0.0f, -torsoSize.y, 0.0f);
			G3D::Vector3 pointOnTorso = cframe.pointToWorldSpace(offset);
			G3D::Ray tryRay = G3D::Ray::fromOriginAndDirection(pointOnTorso, -G3D::Vector3::unitY());

			PartInstance* part = tryFloor(tryRay, maxDistance);
			if (part)
				return part;

			offset = G3D::Vector3(0.0f, -torsoSize.y, 0.0f);

			for (int x = -1; x <= 1; x += 2)
			{
				offset.x = torsoSize.x * x;

				for (int z = -1; z <= 1; z += 2)
				{
					offset.z = torsoSize.z * z;
					pointOnTorso = cframe.pointToWorldSpace(offset);
					tryRay = G3D::Ray::fromOriginAndDirection(pointOnTorso, -G3D::Vector3::unitY());

					PartInstance* part = tryFloor(tryRay, maxDistance);
					if (part)
						return part;
				}
			}
		}

		return NULL;
	}
}
