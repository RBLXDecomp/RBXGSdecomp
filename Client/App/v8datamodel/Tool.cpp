#include "v8datamodel/Tool.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/JointInstance.h"
#include "v8datamodel/ToolMouseCommand.h"
#include "v8datamodel/Mouse.h"
#include "tool/DragUtilities.h"
#include "humanoid/Humanoid.h"
#include "Network/Players.h"
#include "v8xml/SerializerV2.h"

namespace RBX
{
	Reflection::PropDescriptor<Tool, G3D::CoordinateFrame> prop_Grip("Grip", "Appearance", &Tool::getGrip, &Tool::setGrip, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Tool, G3D::Vector3> prop_GripPos("GripPos", "Appearance", &Tool::getGripPos, &Tool::setGripPos, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Tool, G3D::Vector3> prop_GripForward("GripForward", "Appearance", &Tool::getGripForward, &Tool::setGripForward, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Tool, G3D::Vector3> prop_GripUp("GripUp", "Appearance", &Tool::getGripUp, &Tool::setGripUp, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Tool, G3D::Vector3> prop_GripRight("GripRight", "Appearance", &Tool::getGripRight, &Tool::setGripRight, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Tool, int> prop_BackendToolState("BackendToolState", "Appearance", &Tool::getBackendToolState, &Tool::setBackendToolState, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Tool, int> prop_FrontendActivationState("ActivationState", "Appearance", &Tool::getFrontendActivationState, &Tool::setFrontendActivationState, Reflection::PropertyDescriptor::STREAMING);

	Reflection::SignalDesc<Tool, void(boost::shared_ptr<Instance>)> event_Equipped("Equipped", "mouse");
	Reflection::SignalDesc<Tool, void(void)> event_Unequipped("Unequipped");
	Reflection::SignalDesc<Tool, void(void)> event_Activated("Activated");
	Reflection::SignalDesc<Tool, void(void)> event_Deactivated("Deactivated");

	Tool::Tool()
		: backendToolState(NOTHING),
		  frontendActivationState(0),
		  toolMouseCommand(NULL),
		  enabled(true)
	{
		setName("Tool");
	}

	Tool::~Tool()
	{
		RBXASSERT(!handleTouched.connected());
		RBXASSERT(!characterChildAdded.connected());
		RBXASSERT(!characterChildRemoved.connected());
		RBXASSERT(!torsoChildAdded.connected());
		RBXASSERT(!torsoChildRemoved.connected());
	}

	//99.46% match
	//different register allocation
	void Tool::readProperty(const XmlElement* propertyElement, IReferenceBinder& binder)
	{
		const Name* name = NULL;
		if (const XmlAttribute* attrib = propertyElement->findAttribute(name_name))
		{
			if (attrib->getValue(name))
			{
				if (name->toString() == "BackendToolState" || name->toString() == "ActivationState")
					return;
			}
		}

		Instance::readProperty(propertyElement, binder);
	}

	Tool::ToolState Tool::computeDesiredState()
	{
		RBXASSERT(Network::Players::backendProcessing(this, true));

		if (!getHandleConst())
			return NOTHING;

		if (!Workspace::contextInWorkspace(this))
			return HAS_HANDLE;

		return computeDesiredState(getParent());
	}

	Tool::ToolState Tool::computeDesiredState(Instance* testParent)
	{
		Humanoid* humanoid = Humanoid::modelIsCharacter(testParent);
		if (!humanoid)
			return IN_WORKSPACE;

		if (!humanoid->getTorso())
			return IN_CHARACTER;

		if (!humanoid->findRightArm() || !humanoid->findRightShoulder())
			return HAS_TORSO;

		return EQUIPPED;
	}

	void Tool::downFrom_InCharacter()
	{
		characterChildAdded.disconnect();
		characterChildRemoved.disconnect();
	}

	void Tool::downFrom_HasTorso()
	{
		torsoChildAdded.disconnect();
		torsoChildRemoved.disconnect();
	}

	const G3D::Vector3 Tool::getGripPos() const
	{
		return grip.translation;
	}

	const G3D::Vector3 Tool::getGripForward() const
	{
		return grip.lookVector();
	}

	const G3D::Vector3 Tool::getGripUp() const
	{
		return grip.upVector();
	}

	const G3D::Vector3 Tool::getGripRight() const
	{
		return grip.rightVector();
	}

	PartInstance* Tool::getHandle()
	{
		return const_cast<PartInstance*>(getHandleConst());
	}

	const G3D::CoordinateFrame Tool::getLocation() const
	{
		const PartInstance* handle = getHandleConst();
		return handle ? handle->getCoordinateFrame() : G3D::CoordinateFrame();
	}

	void Tool::render3dSelect(Adorn* adorn, SelectState selectState)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			if (IRenderable* renderable = fastDynamicCast<IRenderable>(getChild(i)))
				renderable->render3dSelect(adorn, selectState);
		}
	}

	void Tool::onEvent_AddedBackend(boost::shared_ptr<Instance> child)
	{
		if (child.get() != this)
		{
			RBXASSERT(ServiceProvider::findServiceProvider(this));
			RBXASSERT(Network::Players::backendProcessing(this, true));

			ToolState state = computeDesiredState();
			const ServiceProvider* serviceProvider = ServiceProvider::findServiceProvider(this);

			RBXASSERT(serviceProvider);

			setDesiredState(state, serviceProvider);
		}
	}

	void Tool::onEvent_RemovedBackend(boost::shared_ptr<Instance> child)
	{
		if (ServiceProvider::findServiceProvider(this) && child.get() != this)
		{
			RBXASSERT(Network::Players::backendProcessing(this, true));

			if (child.get() == (Instance*)this->weld.get())
			{
				RBXASSERT(Tool::drawSelected());
				setDesiredState(HAS_TORSO, ServiceProvider::findServiceProvider(this));
			}

			ToolState state = computeDesiredState();
			const ServiceProvider* serviceProvider = ServiceProvider::findServiceProvider(this);

			RBXASSERT(serviceProvider);

			setDesiredState(state, serviceProvider);
		}
	}

	int Tool::getNumToolsInCharacter()
	{
		int total = 0;

		if (Humanoid::modelIsCharacter(getParent()))
		{
			Instance* parent = getParent();

			for (size_t i = 0; i < parent->numChildren(); i++)
			{
				if (fastDynamicCast<Tool>(parent->getChild(i)))
					total++;
			}
		}

		return total;
	}

	void Tool::moveAllToolsToBackpack(Network::Player* player)
	{
		if (player)
		{
			if (ModelInstance* character = player->getCharacter())
			{
				while (Tool* tool = character->findFirstChildOfType<Tool>())
				{
					tool->setParent(player->getPlayerBackpack());
				}
			}
		}
	}

	void Tool::upTo_Equipped()
	{
		handleTouched.disconnect();

		std::vector<boost::weak_ptr<PartInstance>> parts;
		PartInstance::findParts(this, parts);
		DragUtilities::unJoinFromOutsiders(parts);

		Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
		RBXASSERT(humanoid);

		PartInstance* rightArm = humanoid->getRightArm();
		RBXASSERT(rightArm);

		PartInstance* handle = getHandle();
		RBXASSERT(handle);

		buildWeld(rightArm, handle, humanoid->getRightArmGrip(), grip, "RightGrip");
	}

	void Tool::connectTouchEvent()
	{
		if (PartInstance* handle = getHandle())
		{
			boost::slot<boost::function<void(boost::shared_ptr<Instance>)>> slot(boost::bind(&Tool::onEvent_HandleTouched, this, _1));
			handleTouched = PartInstance::event_Touched.connect(handle, slot);
		}
		else
		{
			handleTouched.disconnect();
		}
	}

	void Tool::setBackendToolState(int value)
	{
		if (value != backendToolState)
		{
			setBackendToolStateNoReplicate(value);
			raisePropertyChanged(prop_BackendToolState);
		}
	}

	void Tool::setGrip(const G3D::CoordinateFrame& value)
	{
		if (value != grip)
		{
			grip = value;

			if (Workspace* workspace = ServiceProvider::find<Workspace>(this))
				workspace->raiseDrawChanged();

			raisePropertyChanged(prop_Grip);

			if (weld)
			{
				RBXASSERT(Network::Players::backendProcessing(this, true));
				weld->setC1(grip);
			}
		}
	}

	void Tool::setGripPos(const G3D::Vector3& v)
	{
		setGrip(G3D::CoordinateFrame(grip.rotation, v));
	}

	void Tool::setGripForward(const G3D::Vector3& v)
	{
		G3D::CoordinateFrame c = grip;
		G3D::Vector3 oldX = c.rotation.getColumn(0);
		G3D::Vector3 oldY = c.rotation.getColumn(1);

		G3D::Vector3 z = -v.direction();

		G3D::Vector3 x = oldY.cross(z);
		x.unitize();

		G3D::Vector3 y = z.cross(x);
		y.unitize();
		
		c.rotation.setColumn(0, x);
		c.rotation.setColumn(1, y);
		c.rotation.setColumn(2, z);

		setGrip(c);
	}

	const PartInstance* Tool::getHandleConst() const
	{
		return dynamic_cast<PartInstance*>(findFirstChildByName("Handle"));
	}

	boost::shared_ptr<Mouse> Tool::getMouse()
	{
		boost::shared_ptr<Mouse> mouse;

		if (toolMouseCommand)
			mouse = toolMouseCommand->getMouse();
		else
			mouse = Creatable::create<Mouse>();

		return mouse;
	}

	void Tool::setFrontendActivationState(int value)
	{
		int prevState = frontendActivationState;

		if (value > frontendActivationState)
		{
			frontendActivationState = value;
			raisePropertyChanged(prop_FrontendActivationState);

			if (ServiceProvider::findServiceProvider(this) && Network::Players::backendProcessing(this, true) && Tool::drawSelected())
			{
				if (value - 1 > prevState)
					setBackendToolState((value - 1) % 2 == 0 ? EQUIPPED : ACTIVATED);

				setBackendToolState(value % 2 == 0 ? EQUIPPED : ACTIVATED);
			}
		}
	}

	void Tool::activate()
	{
		RBXASSERT(Network::Players::frontendProcessing(this, true));
		setFrontendActivationState(frontendActivationState + (frontendActivationState % 2) + 1);
	}

	void Tool::deactivate()
	{
		RBXASSERT(Network::Players::frontendProcessing(this, true));
		setFrontendActivationState(frontendActivationState + ((frontendActivationState + 1) % 2) + 1);
	}
}
