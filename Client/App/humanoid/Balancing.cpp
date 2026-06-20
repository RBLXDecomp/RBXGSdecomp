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
}
