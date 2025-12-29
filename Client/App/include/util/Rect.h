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
		//Rect(const Rect&);
		Rect(const G3D::Vector2&, const G3D::Vector2&);
		Rect(const G3D::Vector2&);
		Rect(float, float, float, float);
		Rect(G3D::Rect2D);
		Rect();
	public:
		G3D::Rect2D toRect2D() const;
		void unionWith(const G3D::Vector2&);
		void unionWith(const Rect&);
		bool operator==(const Rect&) const;
		bool pointInRect(G3D::Vector2int16);
		bool pointInRect(int, int);
		G3D::Vector2 size() const;
		G3D::Vector2 center() const;
		Location pointInBorder(const G3D::Vector2&, float);
		G3D::Vector2 positionPoint(const G3D::Vector2&, Location, Location) const;
		G3D::Vector2 positionPoint(Location, Location) const;
		Rect positionChild(const RBX::Rect&, Location, Location) const;
		Rect inset(const G3D::Vector2int16&);
		Rect inset(int);
		G3D::Vector2 clamp(const G3D::Vector2&);
	public:
		//Rect& operator=(const Rect&);

	public:
		static bool legalX(Location);
		static bool legalY(Location);
		static Rect fromLowSize(const G3D::Vector2&, const G3D::Vector2&);
		static Rect fromCenterSize(const G3D::Vector2&, float);
	};
}
