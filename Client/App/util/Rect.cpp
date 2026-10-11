#include "util/Rect.h"

namespace RBX
{
	G3D::Vector2 Rect::positionPoint(const G3D::Vector2& point, Location xLocation, Location yLocation) const
	{
		G3D::Vector2 newPoint;

		switch (xLocation)
		{
		case LEFT:
			newPoint.x = low.x + point.x;
			break;
		case RIGHT:
			newPoint.x = high.x - point.x;
			break;
		case CENTER:
			break;
		}

		switch (yLocation)
		{
		case TOP:
			newPoint.y = low.y + point.y;
			break;
		case BOTTOM:
			newPoint.y = high.y - point.y;
			break;
		case CENTER:
			break;
		}

		return newPoint;
	}

	G3D::Vector2 Rect::positionPoint(Location xLocation, Location yLocation) const
	{
		G3D::Vector2 point = center();

		switch (xLocation)
		{
		case LEFT:
			point.x = low.x;
			break;
		case RIGHT:
			point.x = high.x;
			break;
		case CENTER:
			break;
		}

		switch (yLocation)
		{
		case TOP:
			point.y = low.y;
			return point;
		case BOTTOM:
			point.y = high.y;
			return point;
		case CENTER:
			return point;
		}
	}

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
