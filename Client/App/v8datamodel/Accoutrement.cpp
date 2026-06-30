#include "v8datamodel/Accoutrement.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/JointInstance.h"
#include "tool/DragUtilities.h"
#include "v8xml/SerializerV2.h"
#include "Network/Players.h"

namespace RBX
{
	Reflection::PropDescriptor<Accoutrement, G3D::CoordinateFrame> prop_AttachmentPoint("AttachmentPoint", "Appearance", &Accoutrement::getAttachmentPoint, &Accoutrement::setAttachmentPoint, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Accoutrement, G3D::Vector3> prop_AttachmentPos("AttachmentPos", "Appearance", &Accoutrement::getAttachmentPos, &Accoutrement::setAttachmentPos, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Accoutrement, G3D::Vector3> prop_AttachmentForward("AttachmentForward", "Appearance", &Accoutrement::getAttachmentForward, &Accoutrement::setAttachmentForward, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Accoutrement, G3D::Vector3> prop_AttachmentUp("AttachmentUp", "Appearance", &Accoutrement::getAttachmentUp, &Accoutrement::setAttachmentUp, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Accoutrement, G3D::Vector3> prop_AttachmentRight("AttachmentRight", "Appearance", &Accoutrement::getAttachmentRight, &Accoutrement::setAttachmentRight, Reflection::PropertyDescriptor::UI);
	Reflection::PropDescriptor<Accoutrement, int> prop_BackendAccoutrementState("BackendAccoutrementState", "Appearance", &Accoutrement::getBackendAccoutrementState, &Accoutrement::setBackendAccoutrementState, Reflection::PropertyDescriptor::STREAMING);

	Accoutrement::Accoutrement()
		: backendAccoutrementState(NOTHING)
	{
		setName("Accoutrement");
	}

	Accoutrement::~Accoutrement()
	{
		RBXASSERT(!weld);
		RBXASSERT(!handleTouched.connected());
		RBXASSERT(!characterChildAdded.connected());
		RBXASSERT(!characterChildRemoved.connected());
	}

	//99.29% match
	//see Tool::readProperty
	void Accoutrement::readProperty(const XmlElement* propertyElement, IReferenceBinder& binder)
	{
		const Name* name = NULL;

		if (const XmlAttribute* attrib = propertyElement->findAttribute(name_name))
		{
			if (attrib->getValue(name))
			{
				if (name->toString() == "BackendAccoutrementState")
					return;
			}
		}

		Instance::readProperty(propertyElement, binder);
	}

	void Accoutrement::downFrom_InCharacter()
	{
		characterChildAdded.disconnect();
		characterChildRemoved.disconnect();
	}

	const G3D::Vector3 Accoutrement::getAttachmentPos() const
	{
		return attachmentPoint.translation;
	}

	const G3D::Vector3 Accoutrement::getAttachmentForward() const
	{
		return attachmentPoint.lookVector();
	}

	const G3D::Vector3 Accoutrement::getAttachmentUp() const
	{
		return attachmentPoint.upVector();
	}

	const G3D::Vector3 Accoutrement::getAttachmentRight() const
	{
		return attachmentPoint.rightVector();
	}

	PartInstance* Accoutrement::getHandle()
	{
		return const_cast<PartInstance*>(getHandleConst());
	}

	const G3D::CoordinateFrame Accoutrement::getLocation() const
	{
		const PartInstance* handle = getHandleConst();
		return handle ? handle->getCoordinateFrame() : G3D::CoordinateFrame();
	}

	Accoutrement::AccoutrementState Accoutrement::computeDesiredState()
	{
		RBXASSERT(Network::Players::backendProcessing(this, true));

		if (!getHandleConst())
			return NOTHING;

		if (!Workspace::contextInWorkspace(this))
			return HAS_HANDLE;

		return computeDesiredState(getParent());
	}

	Accoutrement::AccoutrementState Accoutrement::computeDesiredState(Instance* testParent)
	{
		Humanoid* humanoid = Humanoid::modelIsCharacter(testParent);
		if (!humanoid)
			return IN_WORKSPACE;

		if (!humanoid->getTorso())
			return NOTHING;

		return EQUIPPED;
	}

	void Accoutrement::onCameraNear(float distance)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			if (ICameraSubject* subject = fastDynamicCast<ICameraSubject>(getChild(i)))
				subject->onCameraNear(distance);
		}
	}

	void Accoutrement::render3dSelect(Adorn* adorn, SelectState selectState)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			if (IRenderable* renderable = fastDynamicCast<IRenderable>(getChild(i)))
				renderable->render3dSelect(adorn, selectState);
		}
	}

	void Accoutrement::onEvent_AddedBackend(boost::shared_ptr<Instance> child)
	{
		if (child.get() != this)
		{
			RBXASSERT(ServiceProvider::findServiceProvider(this));
			RBXASSERT(Network::Players::backendProcessing(this, true));

			AccoutrementState state = computeDesiredState();
			const ServiceProvider* serviceProvider = ServiceProvider::findServiceProvider(this);

			RBXASSERT(serviceProvider);

			setDesiredState(state, serviceProvider);
		}
	}

	void Accoutrement::onEvent_RemovedBackend(boost::shared_ptr<Instance> child)
	{
		if (child.get() != this)
		{
			RBXASSERT(ServiceProvider::findServiceProvider(this));
			RBXASSERT(Network::Players::backendProcessing(this, true));

			if (child.get() == (Instance*)this->weld.get())
			{
				RBXASSERT(Accoutrement::drawSelected());
				setDesiredState(NOTHING, ServiceProvider::findServiceProvider(this));
			}

			AccoutrementState state = computeDesiredState();
			const ServiceProvider* serviceProvider = ServiceProvider::findServiceProvider(this);

			RBXASSERT(serviceProvider);

			setDesiredState(state, serviceProvider);
		}
	}

	XmlElement* Accoutrement::write()
	{
		if (!writeUnlocked)
			return new XmlElement(tag_External, InstanceHandle(this));
		else
			return Instance::write();
	}

	void Accoutrement::upTo_Equipped()
	{
		handleTouched.disconnect();

		std::vector<boost::weak_ptr<PartInstance>> parts;
		PartInstance::findParts(this, parts);
		DragUtilities::unJoinFromOutsiders(parts);

		Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
		RBXASSERT(humanoid);

		PartInstance* head = humanoid->getHead();
		RBXASSERT(head);

		PartInstance* handle = getHandle();
		RBXASSERT(handle);

		buildWeld(head, handle, humanoid->getTopOfHead(), attachmentPoint, "HeadWeld");
	}

	void Accoutrement::connectTouchEvent()
	{
		if (PartInstance* handle = getHandle())
		{
			boost::slot<boost::function<void(boost::shared_ptr<Instance>)>> slot(boost::bind(&Accoutrement::onEvent_HandleTouched, this, _1));
			handleTouched = PartInstance::event_Touched.connect(handle, slot);
		}
		else
		{
			handleTouched.disconnect();
		}
	}

	void Accoutrement::setBackendAccoutrementState(int value)
	{
		if (value != backendAccoutrementState)
		{
			backendAccoutrementState = (AccoutrementState)value;
			raisePropertyChanged(prop_BackendAccoutrementState);
		}
	}

	void Accoutrement::setAttachmentPoint(const G3D::CoordinateFrame& value)
	{
		if (value != attachmentPoint)
		{
			attachmentPoint = value;

			if (Workspace* workspace = ServiceProvider::find<Workspace>(this))
				workspace->raiseDrawChanged();

			raisePropertyChanged(prop_AttachmentPoint);

			if (weld)
			{
				RBXASSERT(Network::Players::backendProcessing(this, true));
				weld->setC1(attachmentPoint);
			}
		}
	}

	void Accoutrement::setAttachmentPos(const G3D::Vector3& v)
	{
		setAttachmentPoint(G3D::CoordinateFrame(attachmentPoint.rotation, v));
	}

	void Accoutrement::setAttachmentForward(const G3D::Vector3& v)
	{
		G3D::CoordinateFrame c = attachmentPoint;
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

		setAttachmentPoint(c);
	}

	const PartInstance* Accoutrement::getHandleConst() const
	{
		return dynamic_cast<PartInstance*>(findFirstChildByName("Handle"));
	}
}
