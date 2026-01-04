#include "v8world/JointBuilder.h"
#include "v8world/RotateJoint.h"
#include "v8world/WeldJoint.h"
#include "v8world/SnapJoint.h"
#include "v8world/GlueJoint.h"

namespace RBX
{
	Joint* JointBuilder::canJoin(Primitive* p0, Primitive* p1)
	{
		const Extents& fuzzyExtent1 = p0->getFastFuzzyExtents();
		const Extents& fuzzyExtent2 = p1->getFastFuzzyExtents();

		if (fuzzyExtent1.separatedByMoreThan(fuzzyExtent2, 0.05f))
		{
			return NULL;
		}

		const G3D::CoordinateFrame& c0 = p0->getCoordinateFrame();
		const G3D::CoordinateFrame& c1 = p1->getCoordinateFrame();

		for (int i = 0; i < 6; i++)
		{	
			int tempI = i % 3;
			G3D::Vector3 c0Rotation;
			c0Rotation.x = c0.rotation[0][tempI];
			c0Rotation.y = c0.rotation[1][tempI];
			c0Rotation.z = c0.rotation[2][tempI];
			c0Rotation *= ((i / 3) * -2 + 1);
			
			NormalId id2 = Math::getClosestObjectNormalId(-c0Rotation, c1.rotation);
			Joint* joint = RotateJoint::canBuildJoint(p0, p1, (NormalId)i, id2);

			if (joint)
				return joint;

			joint = WeldJoint::canBuildJoint(p0, p1, (NormalId)i, id2);

			if (joint)
				return joint;

			joint = SnapJoint::canBuildJoint(p0, p1, (NormalId)i, id2);

			if (joint)
				return joint;

			joint = GlueJoint::canBuildJoint(p0, p1, (NormalId)i, id2);

			if (joint)
				return joint;
		}
		return NULL;
	}
}