#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
#include <G3D/Ray.h>

namespace RBX
{
	class Geometry
	{
	public:
		enum GeometryType
		{
			GEOMETRY_BALL = 1,
			GEOMETRY_BLOCK = 2,
			GEOMETRY_NONE = 3,
		};
	protected:
		G3D::Vector3 gridSize;
	protected:
		virtual void onSetSize() {}
	public:
		//Geometry(const Geometry& other);
		Geometry() : gridSize() {}
		virtual ~Geometry() {}

		virtual GeometryType getGeometryType() const
		{
			return GEOMETRY_NONE;
		}

		virtual void setGridSize(const G3D::Vector3& _gridSize)
		{
			this->gridSize = _gridSize;
			this->onSetSize();
		}

		const G3D::Vector3& getGridSize() const
		{
			return this->gridSize;
		}

		virtual float getRadius() const
		{
			return 0.0f;
		}

		virtual G3D::Vector3 getCenterToCorner(const G3D::Matrix3& rotation) const
		{
			return G3D::Vector3::zero();
		}

		virtual G3D::Matrix3 getMoment(float mass) const
		{
			return G3D::Matrix3::zero();
		}

		virtual float getGridVolume() const
		{
			return 0.0f;
		}

		virtual bool hitTest(const G3D::Ray& rayInMe, G3D::Vector3& localHitPoint, bool& inside)
		{
			return false;
		}

		//Geometry& operator=(const RBX::Geometry& other);
		
		static Geometry* nullGeometry()
		{
			static Geometry g;
			return &g;
		}
	};
}
