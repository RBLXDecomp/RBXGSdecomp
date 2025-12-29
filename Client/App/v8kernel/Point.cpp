#include "v8kernel/Point.h"
#include "v8kernel/Body.h"

using namespace RBX;

Point::Point(Body* _body)
	:numOwners(1),
	body(_body ? _body : Body::getWorldBody()),
	localPos(0, 0, 0),
	worldPos(0, 0, 0),
	force(0, 0, 0)
	{}

void Point::step()
{
	this->worldPos = this->body->getPV().position.pointToWorldSpace(this->localPos);
	this->force = G3D::Vector3::zero();
}

void Point::forceToBody()
{
	this->body->accumulateForce(this->force, this->worldPos);
}

void Point::setWorldPos(const G3D::Vector3& _worldPos)
{
	this->worldPos = _worldPos;
	this->localPos = this->body->getPV().position.pointToObjectSpace(this->worldPos);
}
