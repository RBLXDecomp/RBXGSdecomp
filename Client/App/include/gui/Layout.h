#pragma once
#include "util/Rect.h"
#include <G3D/Color4.h>

namespace RBX
{
	class Layout
	{
	public:
		enum Style
		{
			HORIZONTAL,
			VERTICAL
		};

	public:
		Rect::Location xLocation;
		Rect::Location yLocation;
		G3D::Vector2int16 offset;
		Style layoutStyle;
		G3D::Color4 backdropColor;

	public:
		Layout()
			: backdropColor(G3D::Color4::clear()),
			  offset(0, 0),
			  layoutStyle(HORIZONTAL),
			  xLocation(Rect::LEFT),
			  yLocation(Rect::TOP)
		{
		}
	};
}
