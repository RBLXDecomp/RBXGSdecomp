#include "v8world/Joint.h"
#include "v8world/Primitive.h"
#include "v8world/World.h"
#include "util/Face.h"
#include "util/Math.h"

namespace RBX
{
	Joint::Joint(
		Primitive* prim0,
		Primitive* prim1,
		const G3D::CoordinateFrame& _jointCoord0,
		const G3D::CoordinateFrame& _jointCoord1)
		: Edge(prim0, prim1),
		  jointOwner(NULL),
		  active(false)
	{
		jointCoord0 = Math::snapToGrid(_jointCoord0, 0.1f);
		jointCoord1 = Math::snapToGrid(_jointCoord1, 0.1f);
	}

	Joint::Joint()
		: Edge(NULL, NULL),
		  jointOwner(NULL),
		  active(false),
		  jointCoord0(),
		  jointCoord1()
	{
		RBXASSERT(jointCoord0 == G3D::CoordinateFrame());
		RBXASSERT(jointCoord1 == G3D::CoordinateFrame());
	}

	Joint::~Joint()
	{
		RBXASSERT(!jointOwner);
	}

	void Joint::setPrimitive(int i, Primitive* p)
	{
		if (p != getPrimitive(i))
		{
			World* world = getWorld();
			if (world && getPrimitive(i))
				world->onJointPrimitiveNulling(this, getPrimitive(i));
			Edge::setPrimitive(i, p);
			if (world && p)
				world->onJointPrimitiveSet(this, p);
		}
	}

	void Joint::setJointOwner(IJointOwner* value)
	{
		RBXASSERT((value == NULL) != (jointOwner == NULL));
		jointOwner = value;
	}

	IJointOwner* Joint::getJointOwner() const
	{
		return jointOwner;
	}

	void Joint::setJointCoord(int i, const G3D::CoordinateFrame& c)
	{
		if (c != getJointCoord(i))
		{
			if (i == 0)
				jointCoord0 = c;
			else
				jointCoord1 = c;

			World* world = getWorld();
			if (world)
			{
				world->removeJoint(this);
				world->insertJoint(this);
			}
		}
	}

	bool Joint::canBuildJoint(Primitive* p0, Primitive* p1, NormalId nId0, NormalId nId1, float angleMax, float planarMax)
	{
		if (!Math::fuzzyAxisAligned(p0->getCoordinateFrame().rotation, p1->getCoordinateFrame().rotation, angleMax))
			return false;

		Face f0 = p0->getFaceInWorld(nId0);
		Face f1 = p1->getFaceInWorld(nId1);
		if (!Face::hasOverlap(f0, f1, 0.35f))
			return false;

		if (!Face::overlapWithinPlanes(f0, f1, planarMax))
			return false;

		return true;
	}

	bool Joint::canBuildJointLoose(Primitive* p0, Primitive* p1, NormalId nId0, NormalId nId1)
	{
		return canBuildJoint(p0, p1, nId0, nId1, 0.05f, 0.05f);
	}

	bool Joint::canBuildJointTight(Primitive* p0, Primitive* p1, NormalId nId0, NormalId nId1)
	{
		return canBuildJoint(p0, p1, nId0, nId1, 0.01f, 0.01f);
	}

	bool Joint::isAssemblyJoint(Edge* e)
	{
		return getJointType(e) >= ANCHOR_JOINT;
	}
}
