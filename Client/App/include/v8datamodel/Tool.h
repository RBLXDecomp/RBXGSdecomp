#pragma once
#include "v8datamodel/LocalBackpack.h"
#include "v8datamodel/IEquipable.h"
#include "util/IRenderable.h"
#include "util/ILocation.h"
#include "util/ISelectable3d.h"

namespace RBX
{
	namespace Network
	{
		class Player;
	}
	class ModelInstance;
	class Mouse;
	class ToolMouseCommand;

	extern const char* sTool;
	class Tool : public DescribedCreatable<Tool, BackpackItem, &sTool>,
				 public IEquipable,
				 public IRenderable,
				 public virtual ILocation,
				 public virtual ISelectable3d
	{
	private:
		enum ToolState
		{
			NOTHING,
			HAS_HANDLE,
			IN_WORKSPACE,
			IN_CHARACTER,
			HAS_TORSO,
			EQUIPPED,
			ACTIVATED
		};

	private:
		ToolState backendToolState;
		int frontendActivationState;
		G3D::CoordinateFrame grip;
		bool enabled;
		ToolMouseCommand* toolMouseCommand;
		boost::signals::scoped_connection handleTouched;
		boost::signals::scoped_connection characterChildAdded;
		boost::signals::scoped_connection characterChildRemoved;
		boost::signals::scoped_connection torsoChildAdded;
		boost::signals::scoped_connection torsoChildRemoved;
	public:
		static Reflection::BoundProp<bool, 1> prop_Enabled;

	private:
		int getNumToolsInCharacter();
		void onEvent_AddedBackend(boost::shared_ptr<Instance>);
		void onEvent_RemovedBackend(boost::shared_ptr<Instance>);
		void onEvent_HandleTouched(boost::shared_ptr<Instance>);
		boost::shared_ptr<Mouse> getMouse();
		ToolState computeDesiredState(Instance*);
		ToolState computeDesiredState();
		void setDesiredState(ToolState, const ServiceProvider*);
		void setBackendToolStateNoReplicate(int);
		void rebuildBackendState();
		void connectTouchEvent();
		void upTo_Activated();
		void upTo_Equipped();
		void upTo_HasTorso();
		void upTo_InCharacter();
		void upTo_InWorkspace();
		void upTo_HasHandle();
		void downFrom_Activated();
		void downFrom_Equipped();
		void downFrom_HasTorso();
		void downFrom_InCharacter();
		void downFrom_InWorkspace();
		void downFrom_HasHandle();
		void onEquipping();
		void onUnequipped();
		void onChildAdded();
		void onChildRemoved();
		virtual void onAncestorChanged(const AncestorChanged&);
		virtual bool askSetParent(const Instance*) const;
		virtual bool askAddChild(const Instance*) const;
		virtual void readProperty(const XmlElement*, IReferenceBinder&);
		virtual const G3D::CoordinateFrame getLocation() const;
		virtual bool drawSelected() const;
		virtual void onLocalClicked();
		virtual void onLocalOtherClicked();
		virtual void render3dSelect(Adorn*, SelectState);
	public:
		Tool(const Tool&);
		Tool();
		virtual ~Tool();
	public:
		virtual bool canUnequip();
		virtual bool canBePickedUpByPlayer(Network::Player*);
		PartInstance* getHandle();
		const PartInstance* getHandleConst() const;
		void setFrontendActivationState(int);
		int getFrontendActivationState() const;
		void setBackendToolState(int);
		int getBackendToolState() const;
		const G3D::CoordinateFrame& getGrip() const;
		void setGrip(const G3D::CoordinateFrame&);
		void activate();
		void deactivate();
		const G3D::Vector3 getGripPos() const;
		const G3D::Vector3 getGripForward() const;
		const G3D::Vector3 getGripUp() const;
		const G3D::Vector3 getGripRight() const;
		void setGripPos(const G3D::Vector3&);
		void setGripForward(const G3D::Vector3&);
		void setGripUp(const G3D::Vector3&);
		void setGripRight(const G3D::Vector3&);
	public:
		//Tool& operator=(const Tool&);

	private:
		static void moveAllToolsToBackpack(Network::Player*);
		static ToolState characterCanPickUpTool(Instance*);
		static bool characterCanUnequipTool(ModelInstance*);
	public:
		static void dropAll(ModelInstance*);
	};
}
