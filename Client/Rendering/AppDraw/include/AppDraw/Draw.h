#pragma once
#include "RbxGraphics/Adorn.h"
#include "AppDraw/Part.h"
#include <G3D/Rect2D.h>

namespace RBX
{
	class Draw
	{
	private:
		static void spokes(float, float, Adorn*);
		static void adornSurfaces(const Part&, Adorn*, const G3D::Color3&);
		static void adornCylinder(const Part&, Adorn*);
		static void frameBox(const Part&, Adorn*, const G3D::Color4&);
		static void constraint(const Part&, Adorn*, int, const G3D::Color3&);
	public:
		static const G3D::Color3& selectColor();
		static void selectionSquare(const G3D::Rect2D&, float);
		static void partAdorn(const Part&, Adorn*, const G3D::Color3&);
		static void selectionBox(const Part&, Adorn*, SelectState);
		static void selectionBox(const Part&, Adorn*, const G3D::Color4&);
	};
}
