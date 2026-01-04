#include "v8world/Ball.h"
#include "util/Math.h"
#include <G3D/Sphere.h>
#include <G3D/CollisionDetection.h>

namespace RBX
{
	G3D::Matrix3 Ball::getMomentSolid(float mass) const
	{
		float value = this->realRadius * this->realRadius * mass * 0.4f;
		return Math::fromDiagonal(G3D::Vector3(value, value, value));
	}

	void Ball::onSetSize()
	{
		this->realRadius = this->gridSize.x * 0.5f;
	}

	float Ball::getGridVolume() const
	{
		// volume of a sphere = 4/3 * pi * radius^3
		float radius = this->gridSize.x * 0.5f;
		return (4.0f/3.0f) * Math::pi() * radius * radius * radius;
	}

	bool Ball::hitTest(const G3D::Ray& rayInMe, G3D::Vector3& localHitPoint, bool& inside)
	{
		float mag = rayInMe.origin.squaredMagnitude();
		inside = this->realRadius * this->realRadius >= mag;

		G3D::Sphere sphere(G3D::Vector3::zero(), this->realRadius);
		return G3D::CollisionDetection::collisionTimeForMovingPointFixedSphere(
			rayInMe.origin,
			rayInMe.direction,
			sphere,
			localHitPoint) != G3D::inf();
	}
}
