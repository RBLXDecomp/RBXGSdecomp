#include "v8datamodel/JointsService.h"
#include "v8datamodel/Workspace.h"

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

	void JointsService::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Notifier<World,AutoJoin>::disconnect(world, this);
		Notifier<World,AutoDestroy>::disconnect(world, this);

		world = NULL;
		Instance::onServiceProvider(oldProvider, newProvider);
		if (Workspace* workspace = ServiceProvider::find<Workspace>(newProvider))
			world = workspace->getWorld();

		Notifier<World,AutoJoin>::connect(world, this);
		Notifier<World,AutoDestroy>::connect(world, this);
	}
}
