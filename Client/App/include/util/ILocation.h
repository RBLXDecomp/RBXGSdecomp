#pragma once
#include <g3d/coordinateframe.h>

namespace RBX
{
	class __declspec(novtable) ILocation
	{
	public:
		virtual const G3D::CoordinateFrame getLocation() const = 0;
	};
}
