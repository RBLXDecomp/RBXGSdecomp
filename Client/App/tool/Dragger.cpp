#include "tool/Dragger.h"
#include "tool/DragUtilities.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/ContactManager.h"
#include "util/Math.h"

namespace RBX
{
	G3D::Vector3 Dragger::toGrid(const G3D::Vector3& v)
	{
		return Math::toGrid(v, dragSnap());
	}

	void Dragger::movePrimitives(const G3D::Array<Primitive*>& primitives, const G3D::Vector3& delta)
	{
		RBXASSERT(primitives.size() > 0);

		if (delta != G3D::Vector3::zero())
		{
			for (int i = 0; i < primitives.size(); i++)
			{
				Primitive* prim = primitives[i];
				PartInstance* part = PartInstance::fromPrimitive(prim);

				DragUtilities::moveAndClean(part, delta);
			}
		}
	}

	Extents Dragger::computeExtents(const G3D::Array<Primitive*>& primitives)
	{
		RBXASSERT(primitives.size() > 0);

		Extents e = Extents::negativeInfiniteExtents();

		for (int i = 0; i < primitives.size(); i++)
		{
			Primitive* prim = primitives[i];

			Extents worldExtents = prim->getExtentsLocal().toWorldSpace(prim->getCoordinateFrame());
			e.unionWith(worldExtents);
		}

		return e;
	}

	bool Dragger::intersectingWorldOrOthers(const G3D::Array<Primitive*>& primitives, ContactManager& contactManager)
	{
		RBXASSERT(primitives.size() > 0);

		Extents group = computeExtents(primitives);
		return group.min().y < -0.01f || contactManager.intersectingOthers(primitives, 0.01f);
	}

	void Dragger::movePrimitivesDelta(const G3D::Array<Primitive*>& primitives, const G3D::Vector3& delta, G3D::Vector3& movedSoFar)
	{
		RBXASSERT(delta == toGrid(delta));
		RBXASSERT(delta != G3D::Vector3::zero());

		movePrimitives(primitives, delta);
		movedSoFar = toGrid(movedSoFar + delta);
	}

	G3D::Vector3 Dragger::safeMoveYDrop(const G3D::Array<Primitive*>& primitives, const G3D::Vector3& tryMove, ContactManager& contactManager)
	{
		if (primitives.size() == 0)
		{
			RBXASSERT(0);
			return G3D::Vector3::zero();
		}

		G3D::Vector3 moved = tryMove - G3D::Vector3(0.0f, 1.2f, 0.0f);
		moved = Math::toGrid(moved, 0.1f);

		movePrimitives(primitives, moved);

		if (intersectingWorldOrOthers(primitives, contactManager))
			searchUpGross(primitives, moved, contactManager);
		else
			searchDownGross(primitives, moved, contactManager);

		RBXASSERT(moved == Math::toGrid(moved, 0.1f));

		return moved;
	}

	G3D::Vector3 Dragger::safeMoveNoDrop(const G3D::Array<Primitive*>& primitives, const G3D::Vector3& tryMove, ContactManager& contactManager)
	{
		G3D::Vector3 movedSoFar(0.0f, 0.0f, 0.0f);

		if (primitives.size() == 0)
		{
			RBXASSERT(0);
			return G3D::Vector3::zero();
		}

		if (tryMove != G3D::Vector3::zero())
		{
			movePrimitives(primitives, tryMove);
			movedSoFar += tryMove;
		}

		if (intersectingWorldOrOthers(primitives, contactManager))
			searchUpGross(primitives, movedSoFar, contactManager);

		return movedSoFar;
	}

	void Dragger::safeRotate(const G3D::Array<Primitive*>& primitives, const G3D::Matrix3& rotate, ContactManager& contactManager)
	{
		if (primitives.size() == 0)
		{
			RBXASSERT(0);
			return;
		}

		G3D::Vector3 center = toGrid(computeExtents(primitives).center());
		G3D::CoordinateFrame centerCoord(center);
		G3D::CoordinateFrame rotationCoord(rotate, G3D::Vector3::zero());

		for (int i = 0; i < primitives.size(); i++)
		{
			Primitive* prim = primitives[i];

			G3D::CoordinateFrame primInCenter = centerCoord.toObjectSpace(prim->getCoordinateFrame());
			G3D::CoordinateFrame newPrimInCenter = rotationCoord * primInCenter;
			G3D::CoordinateFrame primInWorld = centerCoord * newPrimInCenter;

			PartInstance* part = PartInstance::fromPrimitive(prim);
			part->setCoordinateFrame(primInWorld);
		}
	}
}
