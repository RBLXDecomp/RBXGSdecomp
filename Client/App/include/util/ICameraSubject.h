#pragma once
#include <g3d/coordinateframe.h>
#include "util/ILocation.h"

namespace RBX
{
	class Primitive;
	class __declspec(novtable) ICameraSubject : public virtual ILocation
	{
	public:
		ICameraSubject()
		{
		}
		virtual ~ICameraSubject()
		{
		}
	public:
		virtual void onCameraNear(float distance)
		{
		}
		virtual void getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives)
		{
		}
		virtual float getLerp() const
		{
			return 0.9f;
		}
		virtual bool zoom(const float in, G3D::CoordinateFrame& cameraGoal, G3D::CoordinateFrame& cameraFocus)
		{
			return false;
		}
		// TODO: check match
		virtual void stepGoalAndFocus(G3D::CoordinateFrame& cameraGoal, G3D::CoordinateFrame& cameraFocus, bool cameraExternallyAdjusted)
		{
			cameraFocus = getLocation();
		}
	};
}
