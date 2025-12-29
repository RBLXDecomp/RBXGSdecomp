#include "util/RunningAverage.h"
#include "util/Math.h"

namespace RBX
{
	void RunningAverageState::update(const G3D::CoordinateFrame& cofm, float radius)
	{
		this->position = this->position * 0.75f + cofm.translation * 0.25f;
		this->angles = this->angles * 0.75f + Quaternion(cofm.rotation) * (radius * 0.25f);
	}

	bool RunningAverageState::withinTolerance(const G3D::CoordinateFrame& cofm, float radius, float tolerance)
	{
		G3D::Vector3 deltaPos = Math::vector3Abs(this->position - cofm.translation);
		if (Math::maxAxisLength(deltaPos) > tolerance)
			return false;
		Quaternion deltaAngles = Quaternion(cofm.rotation) * radius;
		if ((this->angles - deltaAngles).maxComponent() > tolerance)
			return false;
		return true;
	}
}