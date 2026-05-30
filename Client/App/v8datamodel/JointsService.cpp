#include "v8datamodel/JointsService.h"

namespace RBX
{
	void JointsService::onEvent(const World* source, AutoDestroy event)
	{
		RBXASSERT(event.joint != NULL);

		IJointOwner* jointOwner = event.joint->getJointOwner();
		RBXASSERT(jointOwner);

		if (jointOwner)
			static_cast<JointInstance*>(jointOwner)->setParent(NULL);
	}

	void JointsService::onEvent(const World* source, AutoJoin event)
	{
		RBXASSERT(event.joint != NULL);
		RBXASSERT(event.joint->getJointOwner() == NULL);

		boost::shared_ptr<JointInstance> ji;
		
		switch (event.joint->getJointType())
		{
		case Joint::SNAP_JOINT:
			ji = Creatable::create<Snap>(event.joint);
			break;
		case Joint::WELD_JOINT:
			ji = Creatable::create<Weld>(event.joint);
			break;
		case Joint::GLUE_JOINT:
			ji = Creatable::create<Glue>(event.joint);
			break;
		case Joint::ROTATE_JOINT:
			ji = Creatable::create<Rotate>(event.joint);
			break;
		case Joint::ROTATE_P_JOINT:
			ji = Creatable::create<RotateP>(event.joint);
			break;
		case Joint::ROTATE_V_JOINT:
			ji = Creatable::create<RotateV>(event.joint);
			break;
		default:
			RBXASSERT(0);
		}

		ji->setParent(this);
	}
}
