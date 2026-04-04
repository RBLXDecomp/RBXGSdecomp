#pragma once
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Plane.h>
#include <G3D/GCamera.h>
#include "util/NormalId.h"
#include "util/Vector3int32.h"

namespace RBX
{
	class Extents
	{
	private:
		G3D::Vector3 low;
		G3D::Vector3 high;

	public:
		Extents(const Vector3int32& low, const Vector3int32& high) : low(low.toVector3()), high(high.toVector3()) {}
		Extents(const G3D::Vector3& low, const G3D::Vector3& high) : low(low), high(high) {}
		Extents() : low(G3D::Vector3::inf()), high(-G3D::Vector3::inf()) {}

	public:
		bool operator==(const Extents&) const;
		bool operator!=(const Extents&) const;
		const G3D::Vector3& min() const {return this->low;}
		const G3D::Vector3& max() const {return this->high;}
		G3D::Vector3 getCorner(int i) const;
		G3D::Vector3 size() const { return this->high - this->low; }
		G3D::Vector3 center() const { return (this->high + this->low) * 0.5f; }
		G3D::Vector3 bottomCenter() const;
		G3D::Vector3 topCenter() const;
		float longestSide() const;
		float volume() const;
		float areaXZ() const;
		Extents toWorldSpace(const G3D::CoordinateFrame& localCoord);
		Extents express(const G3D::CoordinateFrame& myFrame, const G3D::CoordinateFrame& expressInFrame);
		G3D::Vector3 faceCenter(NormalId faceId) const;
		void getFaceCorners(NormalId faceId, G3D::Vector3& v0, G3D::Vector3& v1, G3D::Vector3& v2, G3D::Vector3& v3) const;
		G3D::Plane getPlane(NormalId normalId) const;
		G3D::Vector3 clip(const G3D::Vector3&) const;
		G3D::Vector3 clamp(const Extents&) const;
		NormalId closestFace(const G3D::Vector3& point);
		void unionWith(const Extents& other);
		void shift(const G3D::Vector3&);
		void scale(float scale)
		{
			this->low *= scale;
			this->high *= scale;
		}
		void expand(float f)
		{
			this->low -= G3D::Vector3(f, f, f);
			this->high += G3D::Vector3(f, f, f);
		}
		G3D::Vector3& operator[](int);
		const G3D::Vector3& operator[](int) const;
		operator G3D::Vector3 *();
		operator const G3D::Vector3 *() const;
		bool contains(const G3D::Vector3& point) const;
		bool overlapsOrTouches(const Extents& other) const;
		bool fuzzyContains(const G3D::Vector3& point, float slop) const;
		bool containedByFrustum(const G3D::GCamera::Frustum& frustum) const;
		bool partiallyContainedByFrustum(const G3D::GCamera::Frustum&) const;
		bool separatedByMoreThan(const Extents& other, float distance) const;

	private:
		static float epsilon();
	public:
		static Extents fromCenterCorner(const G3D::Vector3& center, const G3D::Vector3& corner)
		{
			return Extents(center - corner, center + corner);
		}
		static Extents fromCenterRadius(const G3D::Vector3&, float);
		static Extents vv(const G3D::Vector3& v0, const G3D::Vector3& v1);
		static bool overlapsOrTouches(const Extents&, const Extents&);
		static const Extents& zero();
		static const Extents& unit();
		static const Extents& infiniteExtents();
		static const Extents& negativeInfiniteExtents();
	};
}
