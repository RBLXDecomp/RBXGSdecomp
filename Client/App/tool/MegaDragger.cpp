#include "tool/MegaDragger.h"
#include "tool/DragUtilities.h"
#include "tool/Dragger.h"
#include "v8world/ContactManager.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/RootInstance.h"

namespace RBX
{
	MegaDragger::MegaDragger(PartInstance* mousePartPtr, const std::vector<PVInstance*>& pvInstances, RootInstance* rootInstance)
		: mousePart(shared_from(mousePartPtr)),
		  dragParts(),
		  joined(true),
		  rootInstance(rootInstance),
          contactManager(rootInstance->getWorld()->getContactManager())
	{
		DragUtilities::pvsToParts(pvInstances, dragParts);
	}

	MegaDragger::MegaDragger(PartInstance* mousePartPtr, const std::vector<boost::weak_ptr<PartInstance>>& partArray, RootInstance* rootInstance)
		: mousePart(shared_from(mousePartPtr)),
		  dragParts(partArray),
		  rootInstance(rootInstance),
		  joined(true),
		  contactManager(rootInstance->getWorld()->getContactManager())
	{
	}


	MegaDragger::~MegaDragger()
	{
		if (!joined)
		{
			RBXASSERT(0);
			DragUtilities::joinAndStopDragging(dragParts);
		}
	}

	void MegaDragger::startDragging()
	{
		DragUtilities::unJoinAndSetDragging(dragParts);

		if (PartInstance::nonNullInWorkspace(mousePart.lock()))
		{
			rootInstance->setCameraIgnoreParts(mousePart.lock().get());
		}
		joined = false;
	}

	void MegaDragger::continueDragging()
	{
		RBXASSERT(!joined);
		DragUtilities::unJoinAndSetDragging(dragParts);
	}

	void MegaDragger::finishDragging()
	{
		RBXASSERT(!joined);
		rootInstance->clearCameraIgnoreParts();
		DragUtilities::joinAndStopDragging(dragParts);
		if (DragUtilities::anyPartAlive(dragParts))
		{
			rootInstance->setInsertPoint(DragUtilities::computeExtents(dragParts).topCenter());
		}
		joined = true;
	}

	void MegaDragger::alignAndCleanParts()
	{
		RBXASSERT(!joined);
		if (PartInstance::nonNullInWorkspace(mousePart.lock()))
		{
			PartInstance* part = mousePart.lock().get();

			if (!part->aligned())
			{
				G3D::CoordinateFrame snap = part->worldSnapLocation();
				G3D::CoordinateFrame snapToGrid = Math::snapToGrid(snap, Dragger::dragSnap());
				DragUtilities::move(dragParts, snap, snapToGrid);
			}
		}
		DragUtilities::clean(dragParts);
	}

	G3D::Vector3 MegaDragger::safeMoveYDrop(const G3D::Vector3& tryDrag)
	{
		RBXASSERT(!joined);
		RBXASSERT(tryDrag == Math::toGrid(tryDrag, 0.1f));

		G3D::Array<Primitive*> primitives;
		DragUtilities::partsToPrimitives(dragParts, primitives);

		RBXASSERT(primitives.size() > 0);

		G3D::Vector3 result = Dragger::safeMoveYDrop(primitives, tryDrag, contactManager);

		RBXASSERT(!contactManager.intersectingOthers(primitives, 0.01f));
		return result;
	}

	G3D::Vector3 MegaDragger::safeMoveNoDrop(const G3D::Vector3& tryDrag)
	{
		RBXASSERT(!joined);
		RBXASSERT(tryDrag == Math::toGrid(tryDrag, 0.1f));

		G3D::Array<Primitive*> primitives;
		DragUtilities::partsToPrimitives(dragParts, primitives);

		RBXASSERT(primitives.size() > 0);

		G3D::Vector3 result = Dragger::safeMoveNoDrop(primitives, tryDrag, contactManager);

		RBXASSERT(!contactManager.intersectingOthers(primitives, 0.01f));
		return result;
	}

	void MegaDragger::safeRotate(const G3D::Matrix3& rotMatrix)
	{
		RBXASSERT(!joined);

		G3D::Array<Primitive*> primitives;
		DragUtilities::partsToPrimitives(dragParts, primitives);

		RBXASSERT(primitives.size() > 0);

		Dragger::safeRotate(primitives, rotMatrix, contactManager);

		RBXASSERT(!contactManager.intersectingOthers(primitives, 0.01f));
	}

	bool MegaDragger::mousePartAlive()
	{
		return PartInstance::nonNullInWorkspace(mousePart.lock());
	}

	boost::weak_ptr<PartInstance> MegaDragger::getMousePart()
	{
		RBXASSERT(mousePart.use_count());
		return mousePart;
	}
}