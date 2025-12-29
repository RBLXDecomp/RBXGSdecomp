#pragma once
#include <g3d/coordinateframe.h>
#include <g3d/vector3.h>
#include <g3d/color4.h>
#include "util/SurfaceType.h"
#include "util/Vector6.h"

namespace RBX
{
	extern const char* const category_Part = "Part";

	class Part
	{
	public:
		enum PartType
		{
			BALL_PART,
			BLOCK_PART,
			CYLINDER_PART
		};

	public:
		PartType type;
		G3D::Vector3 gridSize;
		G3D::Color4 color;
		Vector6<SurfaceType> surfaceType;
		G3D::CoordinateFrame coordinateFrame;

	public:
		//Part(const Part&);
		Part(PartType type, const G3D::Vector3& gridSize, const G3D::Color4 color, const Vector6<SurfaceType>& surfaceType, const G3D::CoordinateFrame& c)
			: type(type),
			  gridSize(gridSize),
			  color(color),
			  surfaceType(surfaceType),
			  coordinateFrame(c)
		{
		}
		Part(PartType _type, const G3D::Vector3& _gridSize, const G3D::Color4 _color, const G3D::CoordinateFrame& c)
			: type(_type),
			  gridSize(_gridSize),
			  color(_color),
			  surfaceType(),
			  coordinateFrame(c)
		{
		}
		Part();
		~Part()
		{
		}
	public:
		//Part& operator=(const Part&);
	};
}
