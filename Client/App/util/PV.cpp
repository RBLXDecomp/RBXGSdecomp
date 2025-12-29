#include "util/PV.h"

namespace RBX
{
	Velocity PV::velocityAtPoint(const G3D::Vector3& worldPos) const
	{
		G3D::Vector3 v1 = worldPos - this->position.translation;
		G3D::Vector3 v2 = this->velocity.rotational.cross(v1);
		return Velocity(this->velocity.linear + v2, this->velocity.rotational);
	}

	PV PV::pvAtLocalCoord(const G3D::CoordinateFrame& localCoord) const
	{
		G3D::CoordinateFrame worldPos = this->position * localCoord;
		Velocity worldVel = this->velocityAtPoint(worldPos.translation);
		return PV(worldPos, worldVel);
	}

	PV PV::pvAtLocalOffset(const G3D::Vector3& localOffset) const
	{
		G3D::CoordinateFrame cf(G3D::Matrix3::identity(), localOffset);
		return this->pvAtLocalCoord(cf);
	}
}