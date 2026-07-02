#pragma once
#include "v8tree/Instance.h"
#include "v8datamodel/IEquipable.h"
#include "util/IRenderable.h"
#include "util/ICameraSubject.h"
#include "util/ISelectable3d.h"

namespace RBX
{
	class IReferenceBinder;
	class ModelInstance;
	class PartInstance;

	extern const char* sAccoutrement;

	class Accoutrement : public DescribedCreatable<Accoutrement, Instance, &sAccoutrement>,
						 public IEquipable,
						 public IRenderable,
						 public virtual ICameraSubject,
						 public virtual ISelectable3d
	{
		enum AccoutrementState
		{
			NOTHING,
			HAS_HANDLE,
			IN_WORKSPACE,
			IN_CHARACTER,
			EQUIPPED
		};

	protected:
		AccoutrementState backendAccoutrementState;
		G3D::CoordinateFrame attachmentPoint;
		boost::signals::scoped_connection handleTouched;
		boost::signals::scoped_connection characterChildAdded;
		boost::signals::scoped_connection characterChildRemoved;
	public:
		static bool writeUnlocked;

	protected:
		void onEvent_AddedBackend(boost::shared_ptr<Instance> child);
		void onEvent_RemovedBackend(boost::shared_ptr<Instance>);
		void onEvent_HandleTouched(boost::shared_ptr<Instance>);
		AccoutrementState computeDesiredState(Instance* testParent);
		AccoutrementState computeDesiredState();
		void setDesiredState(AccoutrementState, const ServiceProvider*);
		void setBackendAccoutrementStateNoReplicate(int);
		void rebuildBackendState();
		void connectTouchEvent();
		void upTo_Equipped();
		void upTo_InCharacter();
		void upTo_InWorkspace();
		void upTo_HasHandle();
		void downFrom_Equipped();
		void downFrom_InCharacter();
		void downFrom_InWorkspace();
		void downFrom_HasHandle();
		void onChildAdded();
		void onChildRemoved();
		virtual void onAncestorChanged(const AncestorChanged&);

		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}

		virtual bool askAddChild(const Instance* instance) const
		{
			return true;
		}

		virtual void readProperty(const XmlElement* propertyElement, IReferenceBinder& binder);
		virtual const G3D::CoordinateFrame getLocation() const;

		bool drawSelected() const
		{
			return backendAccoutrementState >= EQUIPPED; // only EQUIPPED
		}

		virtual void render3dSelect(Adorn* adorn, SelectState selectState);
	public:
		Accoutrement();
		virtual ~Accoutrement();
		virtual XmlElement* write();
		PartInstance* getHandle();
		const PartInstance* getHandleConst() const;
		void setBackendAccoutrementState(int value);

		int getBackendAccoutrementState() const
		{
			return backendAccoutrementState;
		}

		const G3D::CoordinateFrame& getAttachmentPoint() const
		{
			return attachmentPoint;
		}

		void setAttachmentPoint(const G3D::CoordinateFrame&);
		virtual void onCameraNear(float distance);
		const G3D::Vector3 getAttachmentPos() const;
		const G3D::Vector3 getAttachmentForward() const;
		const G3D::Vector3 getAttachmentUp() const;
		const G3D::Vector3 getAttachmentRight() const;
		void setAttachmentPos(const G3D::Vector3&);
		void setAttachmentForward(const G3D::Vector3&);
		void setAttachmentUp(const G3D::Vector3&);
		void setAttachmentRight(const G3D::Vector3&);

	protected:
		static AccoutrementState characterCanPickUpAccoutrement(Instance*);
		static void UnequipThis(boost::shared_ptr<Instance>);
	public:
		static void dropAll(ModelInstance*);
		static void dropAllOthers(ModelInstance*, Accoutrement*);
	};

	extern const char* sHat;

	class Hat : public DescribedCreatable<Hat, Accoutrement, &sHat>
	{
	public:
		Hat();
	};
}
