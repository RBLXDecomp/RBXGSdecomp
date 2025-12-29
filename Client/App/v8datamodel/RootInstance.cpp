#include "v8datamodel/RootInstance.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/SpawnLocation.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/Teams.h"
#include "v8datamodel/Hopper.h"
#include "v8datamodel/Backpack.h"
#include "v8datamodel/Tool.h"
#include "v8datamodel/Lighting.h"
#include "v8datamodel/Sky.h"
#include "Network/Players.h"
#include "tool/Dragger.h"
#include "tool/MegaDragger.h"
#include "tool/DragUtilities.h"
#include <boost/bind.hpp>

namespace RBX
{
	const char* sRootInstance = "RootInstance";

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	RootInstance::RootInstance()
		: Base(),
		  ControllersUsed(this, &RootInstance::computeControllersUsed),
		  insertPoint(G3D::Vector3::zero()),
		  viewPort(G3D::Vector2(800.0f, 600.0f)),
		  world(new World())
	{
	}
#pragma warning (pop)

	RootInstance::~RootInstance()
	{
	}

	void RootInstance::onDescendentAdded(Instance* instance)
	{
		ModelInstance::onDescendentAdded(instance);
		ControllersUsed.setDirty();
	}

	void RootInstance::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		ControllersUsed.setDirty();
		ModelInstance::onDescendentRemoving(instance);
	}

	void RootInstance::setInsertPoint(const G3D::Vector3& topCenter)
	{
		insertPoint = Dragger::toGrid(topCenter);
	}

	G3D::Vector3 RootInstance::computeIdeInsertPoint() const
	{
		float tempY = G3D::max(0.0f, insertPoint.y);

		Extents worldExtents = getExtentsWorld();
		insertPoint = insertPoint.clamp(worldExtents.min(), worldExtents.max());
		insertPoint.y = tempY;

		insertPoint = Dragger::toGrid(insertPoint);
		return insertPoint;
	}

	G3D::Vector3 RootInstance::computeCharacterInsertPoint(const Extents& extents) const
	{
		ModelInstance* character = Network::Players::findLocalCharacter(this);
		if (character)
		{
			G3D::Vector3 cameraLook = getCamera()->getCameraCoordinateFrame().lookVector();
			cameraLook.unitize();

			G3D::Vector3 halfSize = extents.size() * 0.5f;

			std::vector<boost::weak_ptr<PartInstance>> partArray;
			PartInstance::findParts(character, partArray);

			Extents modelExtents = DragUtilities::computeExtents(partArray);

			G3D::Vector3 insertPoint = modelExtents.center() + (cameraLook * 7.0f) + (halfSize * cameraLook) * G3D::Vector3(1,0,1);
			insertPoint = Dragger::toGrid(insertPoint);

			return insertPoint;
		}
		else
		{
			RBXASSERT(0);
			return computeIdeInsertPoint();
		}
	}

	void RootInstance::moveSafe(MegaDragger& megaDragger, G3D::Vector3 move, MoveType moveType)
	{
		megaDragger.startDragging();
		megaDragger.alignAndCleanParts();

		if (moveType == MOVE_DROP)
			megaDragger.safeMoveYDrop(move);
		else
			megaDragger.safeMoveNoDrop(move);

		megaDragger.finishDragging();
	}

	void RootInstance::moveSafe(std::vector<boost::weak_ptr<PartInstance>>& partArray, G3D::Vector3 move, MoveType moveType)
	{
		MegaDragger megaDragger(NULL, partArray, this);
		moveSafe(megaDragger, move, moveType);
	}

	void RootInstance::moveToPoint(PVInstance* pv, G3D::Vector3 point)
	{
		if (this->contains(pv))
		{
			RBXASSERT(getPrimaryPart());

			std::vector<PVInstance*> pvInstances;
			pvInstances.push_back(pv);

			G3D::Vector3 v = Dragger::toGrid(point - getLocation().translation);

			MegaDragger megaDragger(getPrimaryPart(), pvInstances, this);
			moveSafe(megaDragger, v, MOVE_NO_DROP);
		}
	}

	void RootInstance::moveToCharacterInsertPoint(std::vector<boost::weak_ptr<PartInstance>>& partArray)
	{
		if (!partArray.empty())
		{
			Extents extents = DragUtilities::computeExtents(partArray);

			G3D::Vector3 characterInsertPoint = computeCharacterInsertPoint(extents);
			characterInsertPoint = characterInsertPoint - extents.center();

			G3D::Vector3 moveToInsert = Dragger::toGrid(characterInsertPoint);
			moveSafe(partArray, moveToInsert, MOVE_DROP);
		}
	}

	void RootInstance::moveToIdeInsertPoint(std::vector<boost::weak_ptr<PartInstance>>& partArray)
	{
		if (!partArray.empty())
		{
			Extents extents = DragUtilities::computeExtents(partArray);

			G3D::Vector3 insertPoint = computeIdeInsertPoint();
			insertPoint = insertPoint - extents.center();

			G3D::Vector3 moveToInsert = Dragger::toGrid(insertPoint);
			moveSafe(partArray, moveToInsert, MOVE_DROP);
		}
	}

	void RootInstance::insertRaw(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent, std::vector<boost::weak_ptr<PartInstance>>& partArray)
	{
		RBXASSERT(requestedParent && this->contains(requestedParent));

		std::for_each(instances.begin(), instances.end(), boost::bind(&Instance::setParent, _1, requestedParent));

		for (size_t i = 0; i < instances.size(); ++i)
			PartInstance::findParts(instances[i].get(), partArray);

		DragUtilities::join(partArray);
	}

	void RootInstance::insertToTree(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent)
	{
		std::vector<boost::weak_ptr<PartInstance>> partArray;
		insertRaw(instances, requestedParent, partArray);

		if (!partArray.empty())
			moveSafe(partArray, G3D::Vector3::zero(), MOVE_NO_DROP);
	}

	void RootInstance::insertCharacterView(const std::vector<boost::shared_ptr<Instance>>& instances, std::vector<boost::weak_ptr<PartInstance>>& partArray)
	{
		RBXASSERT(instances.size() > 0);

		if (instances.size() == 1)
		{
			HopperBin* hopperBin = fastDynamicCast<HopperBin>(instances[0].get());
			if (hopperBin)
			{
				Network::Player* localPlayer = Network::Players::findLocalPlayer(this);
				if (localPlayer)
				{
					hopperBin->setParent(localPlayer->getPlayerBackpack());
					return;
				}
			}
		}

		insertRaw(instances, this, partArray);
		moveToCharacterInsertPoint(partArray);
	}

	void RootInstance::insertIdeView(const std::vector<boost::shared_ptr<Instance>>& instances, std::vector<boost::weak_ptr<PartInstance>>& partArray, PromptMode promptMode)
	{
		RBXASSERT(instances.size() > 0);

		if (instances.size() == 1)
		{
			Instance* instance = instances[0].get();
			if (fastDynamicCast<HopperBin>(instance))
			{
				instance->setParent(ServiceProvider::create<StarterPackService>(this));
				return;
			}
			else if (fastDynamicCast<Tool>(instance))
			{
				if (promptMode == ALLOW_PROMPTS && MessageBoxA(NULL, "Put this tool into the starter pack (otherwise drop into the 3d view)?", "Inserting Tool", MB_YESNO) == IDYES)
				{
					instance->setParent(ServiceProvider::create<StarterPackService>(this));
					return;
				}
			}
		}

		insertRaw(instances, this, partArray);
		if (!partArray.empty())
		{
			moveToIdeInsertPoint(partArray);
			Extents extents = DragUtilities::computeExtents(partArray);
			getCamera()->lookAt(extents.center());
		}
	}

	void RootInstance::insert3dView(const std::vector<boost::shared_ptr<Instance>>& instances, PromptMode promptMode)
	{
		RBXASSERT(instances.size() > 0);

		std::vector<boost::weak_ptr<PartInstance>> partArray;
		if (Network::Players::findLocalCharacter(this))
			insertCharacterView(instances, partArray);
		else
			insertIdeView(instances, partArray, promptMode);

		if (!partArray.empty())
		{
			Extents extents = DragUtilities::computeExtents(partArray);
			getCamera()->zoomExtents(extents, viewPort, Camera::ZOOM_OUT_ONLY);
		}
	}

	void RootInstance::insertHopperBin(HopperBin* h)	
	{
		Network::Player* localPlayer = Network::Players::findLocalPlayer(this);
		if (localPlayer)
			h->setParent(localPlayer->getPlayerBackpack());
		else
			h->setParent(ServiceProvider::create<StarterPackService>(this));
	}

	void RootInstance::insertSpawnLocation(SpawnLocation* s)
	{
		if (!s->neutral)
		{
			Teams* teams = ServiceProvider::create<Teams>(this);
			if (!teams->teamExists(s->getTeamColor()))
			{
				boost::shared_ptr<Team> team = Creatable<Instance>::create<Team>();
				team->setParent(teams);
				team->setTeamColor(s->getTeamColor());
				team->setName(s->getTeamColor().name() + " Team");
			}
		}
	}

	void RootInstance::insertInstances(const std::vector<boost::shared_ptr<Instance>>& instances, Instance* requestedParent, InsertMode insertMode, PromptMode promptMode)
	{
		RBXASSERT(instances.size() > 0);
		RBXASSERT(!requestedParent || this->contains(requestedParent));
		RBXASSERT(!((insertMode == INSERT_TO_3D_VIEW) && (requestedParent != this)));

		std::vector<boost::shared_ptr<Instance>> remaining;
		for (size_t i = 0; i < instances.size(); ++i)
		{
			Instance* instance = instances[i].get();
			if (Sky* sky = fastDynamicCast<Sky>(instance))
			{
				ServiceProvider::create<Lighting>(this)->replaceSky(sky);
			}
			else if (Team* team = fastDynamicCast<Team>(instance))
			{
				instance->setParent(ServiceProvider::create<Teams>(this));
			}
			else if (HopperBin* hopperBin = fastDynamicCast<HopperBin>(instance))
			{
				insertHopperBin(hopperBin);
			}
			else if (SpawnLocation* spawnLocation = fastDynamicCast<SpawnLocation>(instance))	
			{
				insertSpawnLocation(spawnLocation);
				remaining.push_back(instances[i]);
			}
			else
			{
				remaining.push_back(instances[i]);
			}
		}

		if (!remaining.empty())
		{
			if (requestedParent && !this->contains(requestedParent))
				std::for_each(remaining.begin(), remaining.end(), boost::bind(&Instance::setParent, _1, requestedParent));

			switch (insertMode)
			{
			case INSERT_TO_3D_VIEW:
				{
					insert3dView(remaining, promptMode);
				}
				return;

			case INSERT_TO_TREE:
				{
					insertToTree(remaining, requestedParent);
				}
				return;

			case INSERT_RAW:
				{
					std::vector<boost::weak_ptr<PartInstance>> partArray;
					insertRaw(remaining, requestedParent, partArray);
				}
				return;
			}
		}
	}

	ControllerTypeArray RootInstance::computeControllersUsed() const
	{
		ControllerTypeArray array;

		for (size_t i = 0; i < numChildren(); ++i)
		{
			const PVInstance* pvChild = fastDynamicCast<const PVInstance>(getChild(i));
			if (pvChild)
			{
				if (pvChild->isControllable())
					array.setController(pvChild->getControllerType(), true);
			}
		}

		return array;
	}
}
