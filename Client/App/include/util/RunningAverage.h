#pragma once
#include <G3D/CoordinateFrame.h>
#include "util/Quaternion.h"

namespace RBX
{
	class RunningAverageState
	{
	private:
		G3D::Vector3 position;
		Quaternion angles;

	public:
		//RunningAverageState(const RunningAverageState&);
		RunningAverageState()
			: position(),
			  angles()
		{
		}
		void update(const G3D::CoordinateFrame& cofm, float radius);
		bool withinTolerance(const G3D::CoordinateFrame& cofm, float radius, float tolerance);
		//RunningAverageState& operator=(const RunningAverageState&);

	private:
		static float weight();
	public:
		static int stepsToSleep();
	};
}
