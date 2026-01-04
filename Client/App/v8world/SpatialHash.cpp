#include "v8world/SpatialHash.h"
#include "v8world/Primitive.h"
#include "v8world/ContactManager.h"
#include "v8world/World.h"
#include "v8world/Assembly.h"
#include "util/debug.h"
#include <G3D/AABox.h>
#include <G3D/CollisionDetection.h>

namespace RBX
{
	size_t SpatialHash::numBuckets()
	{
		return 0x10000;
	}

	SpatialHash::SpatialHash(World* world, ContactManager* contactManager) 
		:world(world), 
		contactManager(contactManager), 
		extraNodes(0), 
		nodesOut(0), 
		maxBucket(0)
	{
		std::vector<SpatialNode*> temp(numBuckets());
		this->nodes.swap(temp);
	}

	SpatialHash::~SpatialHash()
	{
		RBXASSERT(this->nodes.size() == numBuckets());

		for (size_t i = 0; i < numBuckets(); i++)
		{
			if (this->nodes[i])
			{
				while(this->nodes[i])
				{
					SpatialNode* backup = this->nodes[i];
					this->nodes[i] = this->nodes[i]->nextHashLink;
					delete backup;
					this->nodesOut--;
				}
			}
		}

		while(this->extraNodes)
		{
			SpatialNode* node = this->extraNodes;
			this->extraNodes = node->nextHashLink;
			delete node;
		}

		RBXASSERT(this->nodesOut == 0);
	}

	int SpatialHash::getHash(const Vector3int32& grid)
	{
		int result = grid.x * -0xba3 ^ grid.y * 0x409f ^ grid.z * -0x49;
		result &= (numBuckets()-1);
		RBXASSERT(result >= 0);
		RBXASSERT(result < (int)numBuckets());
		return result;
	}

	Vector3int32 SpatialHash::realToHashGrid(const G3D::Vector3& realPoint)
	{
		return Vector3int32::floor(realPoint * 0.125f);
	}

	Extents SpatialHash::hashGridToRealExtents(const G3D::Vector3& hashGrid)
	{
		return Extents(hashGrid * 8.0f, (hashGrid + G3D::Vector3(1, 1, 1)) * 8.0f);
	}

	void SpatialHash::computeMinMax(const Extents& extents, Vector3int32& min, Vector3int32& max)
	{
		min = SpatialHash::realToHashGrid(extents.min());
		max = SpatialHash::realToHashGrid(extents.max());
	}

	void SpatialHash::removeNodeFromHash(SpatialNode* remove)
	{
		SpatialNode** temp = &this->nodes[remove->hashId];
		while (*temp != remove)
		{
			temp = &(*temp)->nextHashLink;
		}
		*temp = remove->nextHashLink;
	}

	SpatialNode* SpatialHash::findNode(Primitive* p, const Vector3int32& grid)
	{
		int hash = SpatialHash::getHash(grid);
		SpatialNode* result = this->nodes[hash];
		while (result->primitive != p || result->gridId != grid)
		{
			result = result->nextHashLink;
		}
		return result;
	}

	void SpatialHash::destroyNode(SpatialNode* destroy)
	{
		SpatialNode* nextPrimitiveLink = destroy->nextPrimitiveLink;
		SpatialNode* prevPrimitiveLink = destroy->prevPrimitiveLink;

		if (nextPrimitiveLink)
			nextPrimitiveLink->prevPrimitiveLink = prevPrimitiveLink;

		if (prevPrimitiveLink)
			prevPrimitiveLink->nextPrimitiveLink = nextPrimitiveLink;
		else
			destroy->primitive->spatialNodes = nextPrimitiveLink;

		removeNodeFromHash(destroy);

		int destroyHash = destroy->hashId;
		Vector3int32 destroyGrid = destroy->gridId;

		for (SpatialNode* node = nodes[destroyHash]; node != NULL; node = node->nextHashLink)
		{
			if (node->gridId == destroyGrid)
			{
				Primitive* destroyPrim = destroy->primitive;
				Primitive* nodePrim = node->primitive;
				RBXASSERT(nodePrim != destroyPrim);
				if (Primitive::getContact(destroyPrim, nodePrim) && !shareCommonGrid(destroyPrim, nodePrim))
					this->contactManager->onReleasePair(destroyPrim, nodePrim);
			}
		}

		destroy->nextHashLink = this->extraNodes;
		--this->nodesOut;
		this->extraNodes = destroy;
	}

	bool SpatialHash::shareCommonGrid(Primitive* me, Primitive* other)
	{
		SpatialNode* spatialNodes = me->spatialNodes;
		while (true)
		{
			if (!spatialNodes)
				return false;
			int hashId = spatialNodes->hashId;
			SpatialNode* currentNode = this->nodes[hashId];
			while (currentNode)
			{
				if (currentNode->primitive == other && currentNode->gridId == spatialNodes->gridId)
					return true;
				currentNode = currentNode->nextHashLink;
			}
			spatialNodes = spatialNodes->nextPrimitiveLink;
		}
	}

	void SpatialHash::addNode(Primitive* p, const Vector3int32& grid)
	{
		int hash = getHash(grid);

		SpatialNode* node = this->extraNodes;
		++this->nodesOut;
		if (node)
			this->extraNodes = node->nextHashLink;
		else
			node = new SpatialNode();

		node->primitive = p;
		node->gridId = grid;
		node->hashId = hash;

		SpatialNode* oldNodes = p->spatialNodes;
		p->spatialNodes = node;
		node->nextPrimitiveLink = oldNodes;
		node->prevPrimitiveLink = NULL;
		if (oldNodes)
			oldNodes->prevPrimitiveLink = node;

		SpatialNode* linkedNode = this->nodes[hash];
		node->nextHashLink = linkedNode;
		this->nodes[hash] = node;

		int numNodes = 1;
		while (linkedNode)
		{
			++numNodes;

			Primitive* linkedP = linkedNode->primitive;
			if (linkedP != p && linkedNode->gridId == grid)
			{
				if (!Primitive::getContact(p, linkedP))
					this->contactManager->onNewPair(p, linkedP);
			}
			else
			{
				RBXASSERT(grid != linkedNode->gridId);
			}

			linkedNode = linkedNode->nextHashLink;
		}

		this->maxBucket = std::max(this->maxBucket, numNodes);
	}

	void SpatialHash::changeMinMax(Primitive* p, const Extents& change, const Extents& oldBox, const Extents& newBox)
	{
		Vector3int32 min = Vector3int32::floor(change.min());
		Vector3int32 max = Vector3int32::floor(change.max());

		for (int i = min.x; i <= max.x; ++i)
		{
			for (int j = min.y; j <= max.y; ++j)
			{
				for (int k = min.z; k <= max.z; ++k)
				{
					bool inNew = newBox.contains(G3D::Vector3(i, j, k));
					bool inOld = oldBox.contains(G3D::Vector3(i, j, k));

					if (inNew)
					{
						if (!inOld)
							addNode(p, Vector3int32(i, j, k));
					}
					else if (inOld)
					{
						SpatialNode* node = findNode(p, Vector3int32(i, j, k));
						destroyNode(node);
					}
				}
			}
		}
	}

	void SpatialHash::primitiveExtentsChanged(Primitive* p)
	{
		RBXASSERT(p->spatialNodes);
		Vector3int32 newMin;
		Vector3int32 newMax;
		const Extents& fuzzyExtents = p->getFastFuzzyExtents();
		SpatialHash::computeMinMax(fuzzyExtents, newMin, newMax);
		if (newMin != p->oldSpatialMin || newMax != p->oldSpatialMax)
		{
			Extents newBox(newMin, newMax);
			Extents oldBox(p->oldSpatialMin, p->oldSpatialMax);
			if (newBox.overlapsOrTouches(oldBox))
			{
				Extents unionBox(newBox);
				unionBox.unionWith(oldBox);
				this->changeMinMax(p, unionBox, oldBox, newBox);
			}
			else
			{
				this->changeMinMax(p, oldBox, oldBox, newBox);
				this->changeMinMax(p, newBox, oldBox, newBox);
			}
			p->oldSpatialMin = newMin;
			p->oldSpatialMax = newMax;

		}
	}

	void SpatialHash::onPrimitiveExtentsChanged(Primitive* p)
	{
		this->primitiveExtentsChanged(p);
	}

	void SpatialHash::onPrimitiveAdded(Primitive* p)
	{
		RBXASSERT(!p->spatialNodes);
		Vector3int32 newMin;
		Vector3int32 newMax;
		const Extents& fuzzyExtents = p->getFastFuzzyExtents();
		SpatialHash::computeMinMax(fuzzyExtents, newMin, newMax);
		p->oldSpatialMin = newMin;
		p->oldSpatialMax = newMax;
		for (int i = newMin.x; i <= newMax.x; i++)
		{
			for (int j = newMin.y; j <= newMax.y; j++)
			{
				for (int k = newMin.z; k <= newMax.z; k++)
				{
					this->addNode(p, Vector3int32(i, j, k));
				}
			}
		}
	}

	void SpatialHash::onPrimitiveRemoved(Primitive* p)
	{
		for (SpatialNode* node = p->spatialNodes; node != NULL; node = p->spatialNodes)
			destroyNode(node);
	}

	void SpatialHash::onAllPrimitivesMoved()
	{
		const G3D::Array<Primitive*>& primitives = this->world->getPrimitives();
		for (int i = 0; i < primitives.size(); i++)
		{
			Primitive* primitive = primitives[i];
			RBXASSERT(primitive);
			RBXASSERT(primitive->getClump());
			if (primitive->getAssembly()->moving())
				this->primitiveExtentsChanged(primitive);
		}
	}

	void SpatialHash::getPrimitivesInGrid(const Vector3int32& grid, G3D::Array<Primitive*>& found)
	{
		RBXASSERT(found.size() == 0);
		int hash = SpatialHash::getHash(grid);
		SpatialNode* node = this->nodes[hash];
		while (node)
		{
			if (node->gridId == grid)
				found.append(node->primitive);

			node = node->nextHashLink;
		}
	}

	void SpatialHash::getPrimitivesTouchingExtents(const Extents& extents, const Primitive* ignore, G3D::Array<Primitive*>& answer)
	{
		RBXASSERT(answer.size() == 0);
		Vector3int32 min;
		Vector3int32 max;
		SpatialHash::computeMinMax(extents, min, max);
		G3D::Array<Primitive*> foundThisGrid;

		for (int i = min.x; i <= max.x; i++)
		{
			for (int j = min.y; j <= max.y; j++)
			{
				for (int k = min.z; k <= max.z; k++)
				{
					foundThisGrid.fastClear();
					this->getPrimitivesInGrid(Vector3int32(i, j, k), foundThisGrid);
					for (int l = 0; l < foundThisGrid.size(); l++)
					{
						Primitive* primitive = foundThisGrid[l];
						if (primitive != ignore && !answer.contains(primitive))
						{
							if (extents.overlapsOrTouches(primitive->getFastFuzzyExtents()))
								answer.append(primitive);
						}
					}
				}
			}
		}
		RBXASSERT(foundThisGrid.size() < 200);
	}

	bool SpatialHash::getNextGrid(Vector3int32& grid, const G3D::Ray& unitRay, float maxDistance)
	{
		Vector3int32 min;
		Vector3int32 max;

		for (int i = 0; i < 3; i++)
		{
			min[i] = unitRay.direction[i] < 0.0f ? -1 : 0;
			max[i] = unitRay.direction[i] > 0.0f ? 1 : 0;
		}

		G3D::Vector3 gridF = grid.toVector3();
		float distanceSquared = G3D::square(maxDistance + 16.0f);

		for (int idfk = 1; idfk <= 3; idfk++)
		{
			for (int i = min.x; i <= max.x; i++)
			{
				for (int j = min.y; j <= max.y; j++)
				{
					for (int k = min.z; k <= max.z; k++)
					{
						if (abs(i)+abs(j)+abs(k) == idfk)
						{
							G3D::Vector3 offset(i, j, k);
							G3D::Vector3 offsetGridF = (offset + gridF);
							G3D::AABox box(offsetGridF * 8.0f, (offsetGridF + G3D::Vector3(1,1,1)) * 8.0f);
							G3D::Vector3 location;

							bool hit = G3D::CollisionDetection::collisionLocationForMovingPointFixedAABox(
								unitRay.origin,
								unitRay.direction,
								box,
								location);

							if (hit && (location - unitRay.origin).squaredMagnitude() < distanceSquared)
							{
									grid = Vector3int32::floor(offset + gridF);
									return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
}