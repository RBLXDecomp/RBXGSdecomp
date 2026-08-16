#include "util/Rect.h"

namespace RBX
{
	Rect Rect::positionChild(const RBX::Rect& child, Location xLocation, Location yLocation) const
	{
		G3D::Vector2 newLow;
		G3D::Vector2 childSize = child.size();

		switch (xLocation)
		{
		case LEFT:
			newLow.x = low.x;
			break;
		case RIGHT:
			newLow.x = high.x - childSize.x;
			break;
		case CENTER:
			newLow.x = (high.x - childSize.x) * 0.5f;
			break;
		}

		switch (yLocation)
		{
		case TOP:
			newLow.y = low.y;
			break;
		case BOTTOM:
			newLow.y = high.y - childSize.y;
			break;
		case CENTER:
			newLow.y = (high.y - childSize.y) * 0.5f;
			break;
		}

		return fromLowSize(newLow, childSize);
	}
}
