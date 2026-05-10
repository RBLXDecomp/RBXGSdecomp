#include "tool/DragUtilities.h"
#include "tool/Dragger.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	bool DragUtilities::anyPartAlive(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		for (size_t i = 0; i < parts.size(); i++)
		{
			if (PartInstance::nonNullInWorkspace(parts[i].lock()))
				return true;
		}

		return false;
	}

	void DragUtilities::clean(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();

			if (part)
			{
				if (part->aligned())
				{
					const G3D::CoordinateFrame& partCoord = part->getCoordinateFrame();
					G3D::CoordinateFrame snapToGrid = Math::snapToGrid(partCoord, 0.1f);

					if (snapToGrid != partCoord)
						part->setCoordinateFrame(snapToGrid);
				}
			}
		}
	}

	void DragUtilities::move(const std::vector<boost::weak_ptr<PartInstance>>& parts, G3D::CoordinateFrame from, G3D::CoordinateFrame to)
	{
		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();

			if (part)
			{
				G3D::CoordinateFrame coord = from.toObjectSpace(part->getCoordinateFrame());
				Math::orthonormalizeIfNecessary(coord.rotation);

				part->setCoordinateFrame(to * coord);
			}
		}
	}

	Extents DragUtilities::computeExtents(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		G3D::Array<Primitive*> primitives;

		partsToPrimitives(parts, primitives);
		return Dragger::computeExtents(primitives);
	}

	void DragUtilities::getPrimitivesConst(const Instance* instance, std::vector<const Primitive*>& primitives)
	{
		const PartInstance* partInstance = Instance::fastDynamicCast<const PartInstance>(instance);

		if (partInstance)
			primitives.push_back(partInstance->getPrimitive());

		for (size_t i = 0; i < instance->numChildren(); i++)
		{
			getPrimitivesConst(instance->getChild(i), primitives);
		}
	}

	void DragUtilities::instancesToParts(const std::vector<Instance*>& instances, std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		for (size_t i = 0; i < instances.size(); i++)
		{
			PartInstance::findParts(instances[i], parts);
		}
	}

	void DragUtilities::join(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		RBXASSERT(notJoined(parts));
		
		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();
			if (PartInstance::nonNullInWorkspace(part))
				part->join();
		}
	}

	void DragUtilities::joinAndStopDragging(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		joinToOutsiders(parts);

		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();
			if (PartInstance::nonNullInWorkspace(part))
			{
				part->setDragging(false);
				if (parts.size() == 1)
				{
					part->setLinearVelocity(G3D::Vector3::zero());
					part->setRotationalVelocity(G3D::Vector3::zero());
				}
			}
		}
	}

	void DragUtilities::joinToOutsiders(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		G3D::Array<Primitive*> primitives;

		World* world = partsToPrimitives(parts, primitives);
		if (world)
			world->createJointsToWorld(primitives);
	}

	void DragUtilities::moveAndClean(PartInstance* part, const G3D::Vector3& pos)
	{
		RBXASSERT(pos == Math::toGrid(pos, 0.1f));

		G3D::CoordinateFrame partCoord = part->getCoordinateFrame();
		partCoord.translation += pos;

		if (part->aligned())
			partCoord = Math::snapToGrid(partCoord, 0.1f);

		part->setCoordinateFrame(partCoord);
	}

	bool DragUtilities::notJoined(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();
			if (PartInstance::nonNullInWorkspace(part))
			{
				if (part->getPrimitive()->countNumJoints() != 0)
					return false;
			}
		}

		return true;
	}

	World* DragUtilities::partsToPrimitives(const std::vector<boost::weak_ptr<PartInstance>>& parts, G3D::Array<Primitive*>& primitives)
	{
		RBXASSERT(primitives.size() == 0);

		World* world = NULL;

		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();
			if (PartInstance::nonNullInWorkspace(part))
			{
				primitives.push_back(part->getPrimitive());

				if (!world)
					world = Workspace::getMyWorldFast(part.get());

				RBXASSERT(world);
			}
		}

		return world;
	}

	void DragUtilities::pvsToParts(const std::vector<PVInstance*>& pvInstances, std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		for (size_t i = 0; i < pvInstances.size(); i++)
		{
			PartInstance::findParts(pvInstances[i], parts);
		}
	}

	void DragUtilities::unJoinAndSetDragging(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		unJoinFromOutsiders(parts);

		for (size_t i = 0; i < parts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = parts[i].lock();
			if (PartInstance::nonNullInWorkspace(part))
				part->setDragging(true);
		}
	}

	void DragUtilities::unJoinFromOutsiders(const std::vector<boost::weak_ptr<PartInstance>>& parts)
	{
		G3D::Array<Primitive*> primitives;
		World* world = partsToPrimitives(parts, primitives);

		if (world)
			world->destroyJointsToWorld(primitives);
	}
}