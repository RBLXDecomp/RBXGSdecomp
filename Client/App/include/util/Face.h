#pragma once
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include "util/Extents.h"
#include "util/Debug.h"

namespace RBX
{
	class Face
	{
	private:
		G3D::Vector3 c0;
		G3D::Vector3 c1;
		G3D::Vector3 c2;
		G3D::Vector3 c3;

	public:
		//Face(const Face&);
		Face()
			: c0(), c1(), c2(), c3()
		{
		}
		Face(const G3D::Vector3& c0, const G3D::Vector3& c1, const G3D::Vector3& c2, const G3D::Vector3& c3)
			: c0(c0), c1(c1), c2(c2), c3(c3)
		{
		}
	private:
		G3D::Vector3 getAxis(int i) const
		{
			RBXASSERT((i == 0) || (i==1));
			return i == 0 ? getU() : getV();
		}
		void minMax(const G3D::Vector3& point, const G3D::Vector3& normal, float& min, float& max) const;
		Face operator*(const G3D::Vector3&) const;
		Face operator*(float) const;
	public:
		void snapToGrid(float grid);
		const G3D::Vector3& operator[](int i) const;
		G3D::Vector3& operator[](int i);
		G3D::Vector3 getU() const
		{
			return (c1 - c0).direction();
		}
		G3D::Vector3 getV() const
		{
			return (c3 - c0).direction();
		}
		G3D::Vector3 getNormal() const;
		G3D::Vector2 size() const
		{
			return G3D::Vector2((c1 - c0).magnitude(), (c3 - c0).magnitude());
		}
		G3D::Vector3 center() const
		{
			return (c2 + c0) * 0.5f;
		}
		Face toWorldSpace(const G3D::CoordinateFrame& objectCoord) const;
		Face toObjectSpace(const G3D::CoordinateFrame& objectCoord) const;
		Face projectOverlapOnMe(const Face& other) const;
		bool fuzzyContainsInExtrusion(const G3D::Vector3& point, float tolerance) const;
		//Face& operator=(const Face&);

	public:
		static Face fromExtentsSide(const Extents& e, NormalId faceId);
		static bool cornersAligned(const Face& f0, const Face& f1, float tolerance);
		static bool hasOverlap(const Face& f0, const Face& f1, float byAtLeast);
		static bool overlapWithinPlanes(const Face& f0, const Face& f1, float tolerance);
	};
}
