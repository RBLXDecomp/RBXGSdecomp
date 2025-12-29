#pragma once
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include "util/Velocity.h"

namespace RBX {
	class PV
	{
	public:
		G3D::CoordinateFrame position;
		RBX::Velocity velocity;
	public:
		PV(const G3D::CoordinateFrame& _position, const Velocity& _velocity)
			: position(_position),
			  velocity(_velocity)
		{}

		PV(const PV& other)
			: position(other.position),
			  velocity(other.velocity)
		{}

		PV() : position(), velocity() {};
		~PV() {};
	public:
		bool operator==(const PV&) const;
		bool operator!=(const PV&) const;
		PV inverse() const;
		PV toObjectSpace(const PV&) const;
		G3D::Vector3 linearVelocityAtPoint(const G3D::Vector3& pointVec3) const
		{
			return this->velocity.linearVelocityAtOffset(pointVec3 - this->position.translation);
		}
		Velocity velocityAtPoint(const G3D::Vector3&) const;
		Velocity velocityAtLocalOffset(const G3D::Vector3&) const;
		PV pvAtLocalOffset(const G3D::Vector3& localOffset) const;
		PV pvAtLocalCoord(const G3D::CoordinateFrame& localCoord) const;
		PV operator*(const PV&) const;
		PV& operator=(const PV& other)
		{
			this->position = other.position;
			this->velocity = other.velocity;
			return *this;
		}
	};
}