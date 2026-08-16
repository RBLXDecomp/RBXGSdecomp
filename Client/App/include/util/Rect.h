#pragma once
#include <G3D/Vector2.h>
#include <G3D/Vector2int16.h>
#include <G3D/Rect2D.h>

namespace RBX
{
	class Rect
	{
	public:
		enum Location
		{
			TOP,
			BOTTOM,
			LEFT,
			RIGHT,
			CENTER,
			NONE
		};

	public:
		G3D::Vector2 low;
		G3D::Vector2 high;

	public:
		static const float BORDER_RATIO;
		static const float BORDER_RATIO_DRAG;
		static const float BORDER_RATIO_THIN;

	public:
		Rect(const G3D::Vector2& low, const G3D::Vector2& high)
			: low(low),
			  high(high)
		{
		}
		Rect::Rect(const G3D::Vector2& size)
			: low(G3D::Vector2::zero()), // TODO: only a CRC match. function might not be correct
			  high(size)
		{
		}
		Rect(float, float, float, float);
		Rect(G3D::Rect2D rect) 
			: low(rect.x0y0()), 
			  high(rect.x1y1())
		{
		}
		Rect();
	public:
		G3D::Rect2D toRect2D() const
		{
			return G3D::Rect2D::xyxy(low, high);
		}
		void unionWith(const G3D::Vector2&);
		void unionWith(const Rect&);
		bool operator==(const Rect&) const;
		bool pointInRect(G3D::Vector2int16 point)
		{
			return pointInRect(point.x, point.y);
		}
		bool pointInRect(int x, int y)
		{
			return (low.x <= x) && (high.x >= x) && (low.y <= y) && (high.y >= y);
		}
		G3D::Vector2 size() const
		{
			return high - low;
		}
		G3D::Vector2 center() const
		{
			return (low + high) * 0.5f;
		}
		Location pointInBorder(const G3D::Vector2&, float);
		G3D::Vector2 positionPoint(const G3D::Vector2&, Location, Location) const;
		G3D::Vector2 positionPoint(Location, Location) const;
		Rect positionChild(const RBX::Rect& child, Location xLocation, Location yLocation) const;
		Rect inset(const G3D::Vector2int16&);
		Rect inset(int);
		G3D::Vector2 clamp(const G3D::Vector2&);

	public:
		static bool legalX(Location);
		static bool legalY(Location);
		static Rect fromLowSize(const G3D::Vector2& low, const G3D::Vector2& size)
		{
			return Rect(low, low + size);
		}
		static Rect fromCenterSize(const G3D::Vector2&, float);
	};
}
