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
		//Layout(const Layout&);
		Layout() // TODO: this constructor is a guess based on another function that uses Layout class (RelativePanel ctor). is this correct?
			: backdropColor(G3D::Color4::clear()),
			  layoutStyle(HORIZONTAL),
			  xLocation(Rect::LEFT),
			  yLocation(Rect::TOP)
		{
		}
	public:
		//Layout& operator=(const Layout&);
	};
}
