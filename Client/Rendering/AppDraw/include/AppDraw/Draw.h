#pragma once
#include "RbxGraphics/Adorn.h"
#include "AppDraw/Part.h"
#include "AppDraw/SelectState.h"
#include "G3D/Rect2D.h"

namespace RBX
{
	class Draw
	{
	private:
		static void spokes(float radius, float axis, Adorn* adorn);
		static void adornSurfaces(const Part& part, Adorn* adorn, const G3D::Color3& controllerColor);
		static void adornCylinder(const Part&, Adorn*);
		static void frameBox(const Part&, Adorn*, const G3D::Color4&);
		static void constraint(const Part& part, Adorn* adorn, int face, const G3D::Color3& controllerColor);

	public:
		static const G3D::Color3& selectColor();
		static void selectionSquare(const G3D::Rect2D&, float);
		static void partAdorn(const Part& part, Adorn* adorn, const G3D::Color3& controllerColor);
		static void selectionBox(const Part& part, Adorn* adorn, SelectState selectState);
		static void selectionBox(const Part& part, Adorn* adorn, const G3D::Color4& selectColor);
	};
}
