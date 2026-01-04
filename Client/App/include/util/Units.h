#pragma once
#include <G3D/Vector3.h>

namespace RBX 
{
	class Units
	{
		public:
			static float mPerRbx();
			static float rbxPerM();
			static float rbxPerMM();
			static G3D::Vector3 kmsVelocityToRbx(const G3D::Vector3&);
			static G3D::Vector3 kmsAccelerationToRbx(const G3D::Vector3& kmsAccel);
			static G3D::Vector3 kmsForceToRbx(const G3D::Vector3&);
			static float kmsForceToRbx(float);
			static G3D::Vector3 kmsTorqueToRbx(const G3D::Vector3&);
			static float kmsDensityToRbx(float);
			static G3D::Vector3 kmsKRotToRbx(const G3D::Vector3&);
			static G3D::Vector3 kmsKRotDampToRbx(const G3D::Vector3&);
	};
}