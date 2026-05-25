#include "v8datamodel/Mouse.h"
#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/PartInstance.h"
#include <G3D/Ray.h>

namespace RBX
{
	G3D::CoordinateFrame Mouse::getHit() const
	{
		checkActive();

		G3D::CoordinateFrame origin = getOrigin();

		if (command)
			command->getPartByLocalCharacter(lastEvent, origin.translation);

		return origin;
	}

	G3D::CoordinateFrame Mouse::getOrigin() const
	{
		checkActive();

		G3D::Ray ray;

		if (command)
			ray = command->getUnitMouseRay(lastEvent);

		G3D::CoordinateFrame origin(ray.origin);
		origin.lookAt(ray.origin + ray.direction);

		return origin;
	}

	PartInstance* Mouse::getTarget() const
	{
		checkActive();

		if (command)
			return command->getPart(lastEvent, NULL);
		else
			return NULL;
	}
}
