#pragma once
#include "v8datamodel/ModelInstance.h"
#include "v8world/Controller.h"
#include "util/InsertMode.h"
#include <G3D/Rect2D.h>

namespace RBX
{
	class World;
	class PartInstance;
	class HopperBin;
	class SpawnLocation;
	class MegaDragger;

	extern const char* sRootInstance;
	class RootInstance : public Reflection::Described<RootInstance, &sRootInstance, ModelInstance>,
						 public ICameraOwner
	{
	private:
		enum MoveType
		{
			MOVE_DROP,
			MOVE_NO_DROP
		};

	private:
		ComputeProp<ControllerTypeArray, RootInstance> ControllersUsed;
		mutable G3D::Vector3 insertPoint;
	protected:
		G3D::Rect2D viewPort;
		std::auto_ptr<World> world;

	private:
		ControllerTypeArray computeControllersUsed() const;
		G3D::Vector3 computeIdeInsertPoint() const;
		G3D::Vector3 computeCharacterInsertPoint(const Extents& extents) const;
		void moveSafe(std::vector<boost::weak_ptr<PartInstance>>& partArray, G3D::Vector3 move, MoveType moveType);
		void moveSafe(MegaDragger& megaDragger, G3D::Vector3 move, MoveType moveType);
		void moveToCharacterInsertPoint(std::vector<boost::weak_ptr<PartInstance>>& partArray);
		void moveToIdeInsertPoint(std::vector<boost::weak_ptr<PartInstance>>& partArray);
		void insertRaw(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent, std::vector<boost::weak_ptr<PartInstance>>& partArray);
		void insertToTree(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent);
		void insertCharacterView(const std::vector<boost::shared_ptr<Instance>>& instances, std::vector<boost::weak_ptr<PartInstance>>& partArray);
		void insertIdeView(const std::vector<boost::shared_ptr<Instance>>& instances, std::vector<boost::weak_ptr<PartInstance>>& partArray, PromptMode promptMode);
		void insert3dView(const std::vector<boost::shared_ptr<Instance>>& instances, PromptMode promptMode);
		void insertHopperBin(HopperBin* h);
		void insertSpawnLocation(SpawnLocation* s);
	protected:
		virtual void onDescendentAdded(Instance* instance);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance);
		virtual void onLastChildRemoved()
		{
		}
		virtual void onChildControllerChanged()
		{
			ControllersUsed.setDirty();
		}
	public:
		//RootInstance(RootInstance&);
	protected:
		RootInstance();
		virtual ~RootInstance();
	public:
		void insertInstances(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent, InsertMode insertMode, PromptMode promptMode);
		void setInsertPoint(const G3D::Vector3& topCenter);
		void moveToPoint(PVInstance* pv, G3D::Vector3 point);
		ControllerTypeArray getControllersUsed() const;
		World* getWorld();
		const G3D::Rect2D& getViewPort();
	public:
		//RootInstance& operator=(RootInstance&);
	};
}
