#include "v8datamodel/ModelInstance.h"
#include "v8datamodel/PartInstance.h"
#include "humanoid/Humanoid.h"
#include "v8world/Primitive.h"
#include "tool/DragUtilities.h"
#include "AppDraw/Draw.h"

static bool physicalCharacterHacked = false;

namespace RBX
{
	const char* sModel = "Model";

	static const Name& partName = Name::declare(sModel, 2000);

	static Reflection::PropDescriptor<ModelInstance, G3D::CoordinateFrame> desc_ModelInPrimary("ModelInPrimary", "Data", &ModelInstance::getModelInPrimary, &ModelInstance::setModelInPrimary, Reflection::PropertyDescriptor::STREAMING); // L25
	static Reflection::RefPropDescriptor<ModelInstance, PartInstance> primaryPartProp("PrimaryPart", "Data", &ModelInstance::getPrimaryPartInternal, &ModelInstance::setPrimaryPart, Reflection::PropertyDescriptor::STANDARD); // L26
	static Reflection::BoundFuncDesc<ModelInstance, void(), 0> desc_breakJoints(&ModelInstance::breakJoints, "BreakJoints", Reflection::FunctionDescriptor::AnyCaller); // L27
	static Reflection::BoundFuncDesc<ModelInstance, void(), 0> desc_makeJoints(&ModelInstance::makeJoints, "MakeJoints", Reflection::FunctionDescriptor::AnyCaller); // L28

	static Reflection::BoundFuncDesc<ModelInstance, void(G3D::Vector3), 1> model_moveFunctionOld(&PVInstance::moveToPoint, "move", "location", Reflection::FunctionDescriptor::AnyCaller); // L30
	static Reflection::BoundFuncDesc<ModelInstance, void(G3D::Vector3), 1> model_moveFunction(&PVInstance::moveToPoint, "MoveTo", "location", Reflection::FunctionDescriptor::AnyCaller); // L31

	bool ModelInstance::showModelCoord = false;

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	ModelInstance::ModelInstance()
		: Base("Model"),
		  modelInPrimary(),
		  primaryPart(NULL),
		  FlagHeight(this, &ModelInstance::computeFlagHeight),
		  LocalGridExtents(this, &ModelInstance::computeLocalGridExtents),
		  WorldGridExtents(this, &ModelInstance::computeWorldGridExtents)
	{
		if (!physicalCharacterHacked)
		{
			const Name& name = Name::declare("PhysicalCharacter", -1);
			AbstractFactoryProduct<Instance>::getCreators()[&name] = &getCreator();
			physicalCharacterHacked = true;
		}
	}
#pragma warning (pop)

	ModelInstance::~ModelInstance()
	{
		RBXASSERT(!primaryPart);
	}

	void ModelInstance::dirtyAll() const
	{
		LocalGridExtents.setDirty();
		WorldGridExtents.setDirty();
		FlagHeight.setDirty();
	}

	bool ModelInstance::askSetParent(const Instance* instance) const
	{
		return fastDynamicCast<const ModelInstance>(instance) != NULL;
	}

	void ModelInstance::onExtentsChanged() const
	{
		dirtyAll();
		PVInstance::onExtentsChanged();
	}

	void ModelInstance::onDescendentAdded(Instance* instance)
	{
		PVInstance::onDescendentAdded(instance);
		dirtyAll();

		if (instance == candidatePrimaryPart.get())
		{
			RBXASSERT(primaryPart != candidatePrimaryPart.get());
			candidatePrimaryPart.reset();

			updatePrimaryPart(rbx_static_cast<PartInstance*>(instance));
		}

		shouldRenderSetDirty();
	}

	void ModelInstance::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		if (primaryPart == instance.get())
			primaryPart = NULL;

		if (candidatePrimaryPart == instance)
		{
			RBXASSERT(0);
			candidatePrimaryPart.reset();
		}

		dirtyAll();
		shouldRenderSetDirty();
		PVInstance::onDescendentRemoving(instance);
	}

	const Primitive* ModelInstance::getBiggestPrimitive() const
	{
		const PartInstance* primaryPart = getPrimaryPartConst();
		if (primaryPart)
			return primaryPart->getPrimitive();

		return NULL;
	}

	void ModelInstance::updatePrimaryPart(PartInstance* set) const
	{
		if (primaryPart == set)
		{
			RBXASSERT(!set || this == primaryPart->getParent() || (primaryPart->getParent() && primaryPart->getParent()->isDescendentOf(this)));
		}
		else if (set)
		{
			RBXASSERT(!set->getParent() || set->isDescendentOf(this));
			if (primaryPart)
			{
				RBXASSERT(this == primaryPart->getParent() || (primaryPart->getParent() && primaryPart->getParent()->isDescendentOf(this)));
				G3D::Matrix3 oldMe = getLocation().rotation;
				modelInPrimary = G3D::CoordinateFrame(primaryPart->getCoordinateFrame().rotation.transpose() * oldMe, G3D::Vector3::zero());
				primaryPart = set;
			}
			else
			{
				modelInPrimary = G3D::CoordinateFrame();
				primaryPart = set;
			}
		}
		else
		{
			modelInPrimary = G3D::CoordinateFrame();
			primaryPart = NULL;
		}
	}

	void ModelInstance::setPrimaryPart(PartInstance* set)
	{
		if (!set || this == primaryPart->getParent() || (primaryPart->getParent() && primaryPart->getParent()->isDescendentOf(this)))
		{
			candidatePrimaryPart.reset();
			updatePrimaryPart(set);
		}
		else
		{
			candidatePrimaryPart = shared_from(set);

			if (primaryPart)
			{
				modelInPrimary = G3D::CoordinateFrame();
				primaryPart = NULL;
			}
		}

		raisePropertyChanged(primaryPartProp);
	}

	static void makeJ(Instance* instance)
	{
		PartInstance* partInstance = Instance::fastDynamicCast<PartInstance>(instance);
		if (partInstance)
		{
			partInstance->join();
		}
		else
		{
			ModelInstance* modelInstance = Instance::fastDynamicCast<ModelInstance>(instance);
			if (modelInstance)
				modelInstance->for_eachChild(makeJ);
		}
	}

	void ModelInstance::makeJoints()
	{
		for_eachChild(makeJ);
	}

	static void breakJ(Instance* instance)
	{
		PartInstance* partInstance = Instance::fastDynamicCast<PartInstance>(instance);
		if (partInstance)
		{
			partInstance->destroyJoints();
		}
		else
		{
			ModelInstance* modelInstance = Instance::fastDynamicCast<ModelInstance>(instance);
			if (modelInstance)
				modelInstance->for_eachChild(breakJ);
		}
	}

	void ModelInstance::breakJoints()
	{
		for_eachChild(breakJ);
	}

	float ModelInstance::computeFlagHeight() const
	{
		float answer = 6.0f;

		if (primaryPart)
		{
			RBXASSERT(this == primaryPart->getParent() || (primaryPart->getParent() && primaryPart->getParent()->isDescendentOf(this)));
			float primaryPartY = primaryPart->getExtentsWorld().max().y;
			answer = (getExtentsWorld().max().y - primaryPartY) + 6.0f;
		}

		return answer;
	}

	PartInstance* ModelInstance::getPrimaryPartInternal() const
	{
		if (primaryPart)
		{
			RBXASSERT(this == primaryPart->getParent() || (primaryPart->getParent() && primaryPart->getParent()->isDescendentOf(this)));
			return primaryPart;
		}
		else
		{
			float biggest = -1.0f;
			PartInstance* biggestPart = NULL;
			for (size_t i = 0; i < numChildren(); ++i)
			{
				const Instance* child = getChild(i);
				const PVInstance* pvChild = fastDynamicCast<const PVInstance>(child);
				if (pvChild)
				{
					const Primitive* primitive = pvChild->getBiggestPrimitive();
					if (primitive)
					{
						PartInstance* part = PartInstance::fromPrimitive(const_cast<Primitive*>(primitive));
						RBXASSERT(!part || part->isDescendentOf(this));

						float area = part->getExtentsWorld().areaXZ();
						if (area > biggest)
						{
							biggest = area;
							biggestPart = part;
						}
					}
				}
			}

			updatePrimaryPart(biggestPart);
			return biggestPart;
		}
	}

	Extents ModelInstance::computeLocalGridExtents() const
	{
		std::vector<const Primitive*> primitives;
		DragUtilities::getPrimitivesConst(this, primitives);

		Extents answer = Extents::negativeInfiniteExtents();

		for (size_t i = 0; i < primitives.size(); i++)
		{
			const Primitive* primitive = primitives[i];
			Extents localExtents = primitive->getExtentsLocal();
			Extents childExtentsInMe = localExtents.express(primitive->getCoordinateFrame(), getLocation());
			answer.unionWith(childExtentsInMe);
		}

		if (primitives.empty())
			answer = Extents::zero();

		return answer;
	}

	Extents ModelInstance::computeWorldGridExtents() const
	{
		return LocalGridExtents.getValue().express(getLocation(), G3D::CoordinateFrame());
	}

	void ModelInstance::legacyTraverseState(const G3D::CoordinateFrame& parentState)
	{
		RBXASSERT(legacyOffset);

		if (&(*getChildren()))
		{
			int count = (int)getChildren()->size();
			if (count > 0)
			{
				G3D::CoordinateFrame myState = parentState * (*legacyOffset);
				for (int i = 0; i < count; ++i)
				{
					Instance* child = getChild(i);
					PVInstance* pvChild = fastDynamicCast<PVInstance>(child);
					if (pvChild)
						pvChild->legacyTraverseState(myState);
				}
			}
		}
	}

	const G3D::CoordinateFrame ModelInstance::getLocation() const
	{
		if (getPrimaryPartConst())
		{
			G3D::CoordinateFrame cf = primaryPart->getCoordinateFrame();
			return G3D::CoordinateFrame(cf.rotation * modelInPrimary.rotation, cf.translation);
		}

		return G3D::CoordinateFrame();
	}

	bool ModelInstance::hitTest(const G3D::Ray& worldRay, G3D::Vector3& worldHitPoint)
	{
		for (size_t i = 0; i < numChildren(); ++i)
		{
			Instance* child = getChild(i);
			PVInstance* pvChild = fastDynamicCast<PVInstance>(child);
			if (pvChild)
			{
				if (pvChild->hitTest(worldRay, worldHitPoint))
					return true;
			}
		}

		return false;
	}

	bool ModelInstance::shouldRender3dAdorn() const
	{
		return isTopLevelPVInstance() && showModelCoord;
	}

	void ModelInstance::render3dAdorn(Adorn* adorn)
	{
		if (isTopLevelPVInstance())
		{
			if (showModelCoord)
				renderCoordinateFrame(adorn);
		}
	}

	void ModelInstance::render3dSelect(Adorn* adorn, SelectState selectState)
	{
		Extents localExtents = getExtentsLocal();
		G3D::CoordinateFrame location = getLocation();
		location.translation = location.pointToWorldSpace(localExtents.center());

		Part part(Part::BLOCK_PART, localExtents.size(), G3D::Color3::white(), location);
		Draw::selectionBox(part, adorn, selectState);
	}

	void ModelInstance::onCameraNear(float distance)
	{
		for (size_t i = 0; i < getChildren()->size(); ++i)
		{
			Instance* child = getChild(i);
			ICameraSubject* cameraSubject = fastDynamicCast<ICameraSubject>(child);
			if (cameraSubject)
				cameraSubject->onCameraNear(distance);
		}
	}

	void ModelInstance::getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives)
	{
		DragUtilities::getPrimitivesConst(this, primitives);

		Humanoid* humanoid = findFirstChildOfType<Humanoid>();
		if (humanoid)
			humanoid->getIgnorePrims(primitives);
	}
}
