#include "v8kernel/Link.h"
#include "v8kernel/Body.h"

using namespace RBX;

Link::Link():body(NULL) {stateIndex = Body::getNextStateIndex();}

Link::~Link()
{
}

void Link::dirty()
{
	if (body)
	{
		RBXASSERT(body->getLink() == this);
		body->getRoot()->advanceStateIndex();
	}
}

const G3D::CoordinateFrame& Link::getChildInParent() const
{
	int bodyStateIndex = body->getParent()->getStateIndex();

	if (stateIndex != bodyStateIndex)
	{
		computeChildInParent(childInParent);
		stateIndex = bodyStateIndex;
	}
	return childInParent;
}

void RevoluteLink::computeChildInParent(G3D::CoordinateFrame& answer) const
{
    G3D::CoordinateFrame rotatedParentCoord(
        Math::rotateAboutZ(parentCoord.rotation, jointAngle), 
        parentCoord.translation);

    answer = rotatedParentCoord * childCoordInverse;
}
