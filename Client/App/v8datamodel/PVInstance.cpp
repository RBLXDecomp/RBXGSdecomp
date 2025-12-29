#include "v8datamodel/PVInstance.h"
#include "v8datamodel/UserController.h"
#include "v8datamodel/Workspace.h"

namespace RBX
{
	const char* sPVInstance = "PVInstance";

	static Reflection::PropDescriptor<PVInstance, G3D::CoordinateFrame> desc_CoordFrame("CoordinateFrame", "Data", NULL, &PVInstance::setPVGridOffsetLegacy, Reflection::PropertyDescriptor::LEGACY); // L23
	static Reflection::PropDescriptor<PVInstance, bool> prop_ControllerFlagShown("ControllerFlagShown", "Appearance", &PVInstance::getShowControllerFlag, &PVInstance::setShowControllerFlag, Reflection::PropertyDescriptor::STANDARD); // L32
	const Reflection::EnumPropDescriptor<PVInstance, Controller::ControllerType> PVInstance::prop_ControllerType("Controller", "Behavior", &PVInstance::getControllerType, &PVInstance::setControllerType, Reflection::PropertyDescriptor::STANDARD); // L34

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	PVInstance::PVInstance(const char* name)
		: Base(name),
		  IsControllable(this, &PVInstance::computeIsControllable),
		  IsTopFlag(this, &PVInstance::computeIsTopFlag),
		  TopPVController(this, &PVInstance::computeTopPVController)
	{

	}
#pragma warning (pop)

	void PVInstance::moveToPoint(G3D::Vector3 point)
	{
		Workspace* workspace = Workspace::findWorkspace(this);
		if (workspace)
			workspace->moveToPoint(this, point);
	}

	void PVInstance::clearLegacyOffset()
	{
		legacyOffset.reset();

		for (size_t i = 0; i < numChildren(); ++i)
		{
			Instance* child = getChild(i);
			PVInstance* pvChild = fastDynamicCast<PVInstance>(child);
			if (pvChild)
				pvChild->clearLegacyOffset();
		}
	}

	void PVInstance::readProperty(const XmlElement* propertyElement, IReferenceBinder& binder)
	{
		if (propertyElement->getTag() == "Feature")
		{
			const Name* name = NULL;

			bool foundNameAttribute = propertyElement->findAttributeValue(name_name, name);
			RBXASSERT(foundNameAttribute);

			if (*name == "Part")
			{
				readProperties(propertyElement, binder);
				return;
			}
			if (*name == "Item")
			{
				readProperties(propertyElement, binder);
				return;
			}
		}

		return Instance::readProperty(propertyElement, binder);
	}

	PVInstance::~PVInstance()
	{
	}

	void PVInstance::onChildControllerChanged()
	{
		dirtyAll();

		PVInstance* pvParent = fastDynamicCast<PVInstance>(getParent());
		if (pvParent)
			pvParent->onChildControllerChanged();
	}

	void PVInstance::onParentControllerChanged()
	{
		dirtyAll();

		for (size_t i = 0; i < numChildren(); ++i)
		{
			Instance* child = getChild(i);
			PVInstance* pvChild = fastDynamicCast<PVInstance>(child);
			if (pvChild)
				pvChild->onParentControllerChanged();
		}
	}

	void PVInstance::dirtyAll()
	{
		TopPVController.setDirty();
		IsControllable.setDirty();
		IsTopFlag.setDirty();
	}

	void PVInstance::onDescendentAdded(Instance* instance)
	{
		Instance::onDescendentAdded(instance);

		dirtyAll();
	}

	void PVInstance::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		dirtyAll();

		Instance::onDescendentRemoving(instance);
	}

	bool PVInstance::computeIsControllable() const
	{
		for (size_t i = 0; i < numChildren(); ++i)
		{
			const Instance* child = getChild(i);
			const IControllable* controllableChild = fastDynamicCast<const IControllable>(child);
			if (controllableChild)
			{
				if (controllableChild->isControllable())
					return true;
			}
		}

		return false;
	}

	G3D::ReferenceCountedPointer<Controller> PVInstance::computeTopPVController() const
	{
		if (!isTopLevelPVInstance())
		{
			G3D::ReferenceCountedPointer<RBX::Controller> controller = rbx_static_cast<PVInstance*>(getParent())->TopPVController;

			if (controller.notNull())
			{
				if (controller->getControllerType() != Controller::NO_CONTROLLER && !controller->isUserController())
					return controller;
			}
		}

		ControllerService* controllerService = ServiceProvider::create<ControllerService>(this);
		G3D::ReferenceCountedPointer<Controller> controller;
		if (controllerService)
			controller = controllerService->getController(controllerType, this);

		return controller;
	}

	void PVInstance::onChildAdded(Instance* instance)
	{
		PVInstance* pvInstance = fastDynamicCast<PVInstance>(instance);
		if (pvInstance)
		{
			pvInstance->onChildControllerChanged();
			pvInstance->onParentControllerChanged();
		}
	}

	void PVInstance::onChildRemoving(Instance* instance)
	{
		PVInstance* pvInstance = fastDynamicCast<PVInstance>(instance);
		if (pvInstance)
		{
			pvInstance->onChildControllerChanged();
			pvInstance->onParentControllerChanged();
		}
	}

	void PVInstance::renderCoordinateFrame(Adorn* adorn)
	{
		adorn->setObjectToWorldMatrix(getLocation());
		adorn->axes(G3D::Color3::red(), G3D::Color3::green(), G3D::Color3::blue(), 10.0f);
	}

	bool PVInstance::computeIsTopFlag() const
	{
		if (!IsControllable.getValue() || controllerType == Controller::NO_CONTROLLER)
			return false;

		if (isTopLevelPVInstance())
			return true;

		return getTypedParent<PVInstance>()->getTopPVController()->getControllerType() == Controller::NO_CONTROLLER;
	}

	void PVInstance::setPVGridOffsetLegacy(const G3D::CoordinateFrame& _offset)
	{
		RBXASSERT(this->isTopLevelPVInstance());

		if (!legacyOffset)
		{
			legacyOffset.reset(new G3D::CoordinateFrame());
			*legacyOffset = _offset;
		}
	}

	void PVInstance::onExtentsChanged() const
	{
		PVInstance* pvParent = fastDynamicCast<PVInstance>(getParent());
		if (pvParent)
			pvParent->onExtentsChanged();
	}

	void PVInstance::setControllerType(Controller::ControllerType _control)
	{
		if (controllerType != _control)
		{
			controllerType = _control;
			onChildControllerChanged();
			onParentControllerChanged();
			raisePropertyChanged(prop_ControllerType);
		}
	}

	void PVInstance::setShowControllerFlag(bool _showControllerFlag)
	{
		if (showControllerFlag != _showControllerFlag)
		{
			showControllerFlag = _showControllerFlag;
			raisePropertyChanged(prop_ControllerFlagShown);
		}
	}
}
