#pragma once
#include <g3d/vector3.h>
#include <g3d/coordinateframe.h>
#include <g3d/ray.h>
#include "RbxGraphics/Adorn.h"
#include "v8tree/Instance.h"
#include "v8world/Controller.h"
#include "v8datamodel/IPrimaryPart.h"
#include "util/IControllable.h"
#include "util/ILocation.h"
#include "util/ComputeProp.h"
#include "util/Extents.h"

namespace RBX
{
	class Primitive;

	extern const char* sPVInstance;
	class PVInstance : public Reflection::Described<PVInstance, &sPVInstance, Instance>,
					   public IControllable,
					   public virtual IPrimaryPart,
					   public virtual ILocation
	{
	protected:
		ComputeProp<bool, PVInstance> IsControllable;
		ComputeProp<bool, PVInstance> IsTopFlag;
		ComputeProp<G3D::ReferenceCountedPointer<Controller>, PVInstance> TopPVController;
	private:
		bool canSelect;
		Controller::ControllerType controllerType;
		bool showControllerFlag;
	protected:
		boost::scoped_ptr<G3D::CoordinateFrame> legacyOffset;
	public:
		static const Reflection::EnumPropDescriptor<PVInstance, Controller::ControllerType> prop_ControllerType;
	  
	private:
		bool computeIsControllable() const;
		bool computeIsTopFlag() const;
		G3D::ReferenceCountedPointer<Controller> computeTopPVController() const;
		void dirtyAll();
	public:
		//PVInstance(const PVInstance&);
	protected:
		PVInstance(const char* name);
	public:
		virtual ~PVInstance();
	public:
		Controller::ControllerType getControllerType() const
		{
			return controllerType;
		}
		void setControllerType(Controller::ControllerType _control);
		bool getShowControllerFlag() const
		{
			return showControllerFlag;
		}
		void setShowControllerFlag(bool _showControllerFlag);
	protected:
		virtual size_t topHashCode() const
		{
			return 0;
		}
		virtual size_t childHashCode() const
		{
			return 0;
		}
		virtual void onChildAdded(Instance* instance);
		virtual void onChildRemoving(Instance* instance);
		virtual void onDescendentAdded(Instance* instance);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance);
		virtual void readProperty(const XmlElement* propertyElement, IReferenceBinder& binder);
	public:
		virtual bool isControllable() const
		{
			return IsControllable.getValue();
		}
	protected:
		void renderCoordinateFrame(Adorn* adorn);
		void onControllerChanged();
		virtual void onChildControllerChanged();
		virtual void onParentControllerChanged();
		virtual void onExtentsChanged() const;
	public:
		void moveToPoint(G3D::Vector3 point);
		// TODO: does not match
		Controller* getTopPVController() const
		{
			G3D::ReferenceCountedPointer<Controller> controller = TopPVController.getValue();
			if (controller.notNull())
				return controller.pointer();
			else
				return NullController::getStaticNullController();
		}
		bool isChaseable() const
		{
			return getTopPVController()->hasIntelligence() && IsControllable.getValue();
		}
		virtual Extents getExtentsWorld() const = 0;
		virtual Extents getExtentsLocal() const = 0;
		virtual const Primitive* getBiggestPrimitive() const = 0;
		virtual bool hitTest(const G3D::Ray&, G3D::Vector3&) = 0;
		void writeCoordinateFrameData(XmlState*);
		void writeVelocityData(XmlState*);
		const PVInstance* getTopLevelPVParent() const;
		PVInstance* getTopLevelPVParent();
		// TODO: does not match
		bool isTopLevelPVInstance() const
		{
			if (fastDynamicCast<PVInstance>(getParent()))
			{
				RBXASSERT(fastDynamicCast<PVInstance>(getParent()) == getParent());
				return getTypedRoot<PVInstance>() == getParent();
			}
			else
			{
				return true;
			}
		}
		void setPVGridOffsetLegacy(const G3D::CoordinateFrame& _offset);
		G3D::CoordinateFrame* getLegacyOffset();
		void clearLegacyOffset();
		virtual void legacyTraverseState(const G3D::CoordinateFrame& parentState) = 0;
	public:
		//PVInstance& operator=(const PVInstance&);
	};
}
