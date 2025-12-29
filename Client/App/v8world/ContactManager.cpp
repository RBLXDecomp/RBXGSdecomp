#include "v8world/ContactManager.h"
#include "v8world/spatialHash.h" // TODO: move these out maybe?
#include "v8world/World.h"

namespace RBX
{
	bool ContactManager::ignoreBool = false;

	ContactManager::ContactManager(World* world)
	{
		SpatialHash* hash = new SpatialHash(world, this);
		this->spatialHash = hash;

		this->world = world;
	}

	ContactManager::~ContactManager()
	{
		delete this->spatialHash;
	}

	void ContactManager::getPrimitivesTouchingExtents(const Extents& extents, const Primitive* ignore, G3D::Array<Primitive*>& found)
	{
		this->spatialHash->getPrimitivesTouchingExtents(extents, ignore, found);
	}

	void ContactManager::onNewPair(Primitive* p0, Primitive* p1)
	{
		Contact* contact = this->createContact(p0, p1);
		this->world->insertContact(contact);
	}

	void ContactManager::onReleasePair(Primitive* p0, Primitive* p1)
	{
		this->world->destroyContact(Primitive::getContact(p0, p1));
	}

	void ContactManager::onPrimitiveAdded(Primitive* p)
	{
		this->spatialHash->onPrimitiveAdded(p);
	}

	void ContactManager::onPrimitiveRemoved(Primitive* p)
	{
		this->spatialHash->onPrimitiveRemoved(p);
	}

	void ContactManager::onPrimitiveExtentsChanged(Primitive* p)
	{
		this->spatialHash->onPrimitiveExtentsChanged(p);
	}

	void ContactManager::stepWorld()
	{
		this->spatialHash->onAllPrimitivesMoved();
	}

	bool ContactManager::intersectingOthers(Primitive* check, float overlapIgnored)
	{
		std::set<Primitive*> checkSet;
		checkSet.insert(check);
		return intersectingOthers(check, checkSet, overlapIgnored);
	}

	bool ContactManager::intersectingOthers(const G3D::Array<Primitive*>& check, float overlapIgnored)
	{
		std::set<Primitive*> checkSet(check.begin(), check.end());

		for(int i = 0; i < check.size(); i++)
		{
			if(intersectingOthers(check[i], checkSet, overlapIgnored))
				return true;
		}
		return false;
	}

	bool ContactManager::intersectingOthers(Primitive* check, const std::set<Primitive*>& checkSet, float overlapIgnored)
	{
		for(Contact* cur = check->getFirstContact(); cur != NULL; cur = check->getNextContact(cur))
		{
			if(checkSet.find(cur->otherPrimitive(check)) == checkSet.end() && cur->computeIsColliding(overlapIgnored))
				return true;
		}
		return false;
	}

	void ContactManager::onPrimitiveGeometryTypeChanged(Primitive* p)
	{
		G3D::Array<Contact*> newContacts;

		for(Contact* cur = p->getFirstContact(); cur != NULL; cur = p->getFirstContact())
		{
			newContacts.push_back(createContact(cur->getPrimitive(0), cur->getPrimitive(1)));
			world->destroyContact(cur);
		}

		for(int i = 0; i < newContacts.size(); i++)
		{
			world->insertContact(newContacts[i]);
		}
	}

	Primitive* ContactManager::getHit(const G3D::Ray& worldRay, const std::vector<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, bool& inside) const
	{
		G3D::Array<Primitive const*> tempIgnore;
		tempIgnore = *ignorePrim;
		return getHit(worldRay, &tempIgnore, filter, hitPoint, inside);
	}

	Primitive* ContactManager::getHit(const G3D::Ray& worldRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, bool& inside) const
	{
		RBXASSERT(worldRay.direction.magnitude() < 5000.0f);
		world->update();

		bool stopped;

		Primitive* fastHit = getFastHit(worldRay, ignorePrim, filter, hitPoint, inside, stopped);

		if(stopped) 
			fastHit = NULL;

		if(!fastHit) 
		{
			hitPoint = G3D::Vector3::zero();
			inside = false;
		}

		return fastHit;
	}

	Primitive* ContactManager::getFastHit(const G3D::Ray& worldRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPointWorld, bool& inside, bool& stopped) const
	{
		G3D::Array<Primitive*> primitives;
		Vector3int32 grid = SpatialHash::realToHashGrid(worldRay.origin);
		float magnitude = worldRay.direction.magnitude();

		RBXASSERT(magnitude < 5000.0f);

		magnitude = G3D::min(5000.0f, magnitude);

		G3D::Ray unitRay = worldRay.unit();

		do
		{
			primitives.fastClear();
			spatialHash->getPrimitivesInGrid(grid, primitives);
			Primitive* slowHit = getSlowHit(primitives, unitRay, ignorePrim, filter, hitPointWorld, magnitude, inside, stopped);

			if(slowHit)
			{
				Extents hashGrid = SpatialHash::hashGridToRealExtents(grid.toVector3());
				if(hashGrid.fuzzyContains(hitPointWorld, 0.001f))
					return slowHit;
			}
		}
		while(spatialHash->getNextGrid(grid, unitRay, magnitude));

		return NULL;
	}

	Primitive* ContactManager::getHitLegacy(const G3D::Ray& originDirection, const Primitive* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPointWorld, float& distanceToHit, const float& maxSearchDepth) const
	{
		RBXASSERT(originDirection.direction.isUnit());

		G3D::Vector3 maxSearchVec = originDirection.direction * maxSearchDepth;
		G3D::Ray worldRay = G3D::Ray::fromOriginAndDirection(originDirection.origin, maxSearchVec);
		G3D::Array<Primitive const*> ignorePrims;

		if(ignorePrim)
			ignorePrims.push_back(ignorePrim);

		Primitive* hit = getHit(worldRay, &ignorePrims, filter, hitPointWorld, ContactManager::ignoreBool);

		float tempDist;

		if(hit)
		{
			G3D::Vector3 temp = hitPointWorld - worldRay.origin;
			tempDist = temp.magnitude();
		}
		else 
			tempDist = 0.0f;

		distanceToHit = tempDist;

		return hit;
	}

	Primitive* ContactManager::getSlowHit(const G3D::Array<Primitive*>& primitives, const G3D::Ray& unitRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, float maxDistance, bool& inside, bool& stopped) const
	{
		RBXASSERT(unitRay.direction.isUnit());

		Primitive* bestPrimitive = NULL;
		float bestOffset = maxDistance;
		float stopOffset = maxDistance;

		stopped = false;
		inside = false;

		for(int i = 0; i < primitives.size(); i++)
		{
			Primitive* currentPrimitive = primitives[i];

			bool isNotFound = ignorePrim ? ignorePrim->find(currentPrimitive) == ignorePrim->end() : true;
			HitTestFilter::Result hitResult = filter ? filter->filterResult(currentPrimitive) : HitTestFilter::INCLUDE_PRIM;

			if(isNotFound && hitResult != HitTestFilter::IGNORE_PRIM)
			{
				G3D::Vector3 trans = currentPrimitive->getCoordinateFrame().translation;
				float radius = currentPrimitive->getRadius();
				float math1 = unitRay.direction.dot(trans - unitRay.origin);
				G3D::Vector3 math2 = trans - (unitRay.origin + unitRay.direction * math1);
				float dist = math2.magnitude();

				if(dist <= radius)
				{
					G3D::Vector3 thisHitPoint;
					bool insideTemp;
					if(currentPrimitive->hitTest(unitRay, thisHitPoint, insideTemp))
					{
						float thisOffset = unitRay.direction.dot(thisHitPoint - unitRay.origin);
						if(thisOffset > 0.0f)
						{
							switch(hitResult)
							{
							case HitTestFilter::STOP_TEST:
								if(thisOffset < stopOffset)
								{
									stopOffset = thisOffset;
									stopped = true;
								}
								break;
							case HitTestFilter::INCLUDE_PRIM:
								if(thisOffset < bestOffset)
								{
									inside = insideTemp;
									bestOffset = thisOffset;
									hitPoint = thisHitPoint;
									bestPrimitive = currentPrimitive;
								}
								break;
							default: 
								RBXASSERT(0);
							}
						}
					}
				}
			}
		}

		if((stopped && bestPrimitive) && bestOffset < stopOffset)
			stopped = false;

		return bestPrimitive;
	}

	Contact* ContactManager::createContact(Primitive* p0, Primitive* p1)
	{
		Geometry::GeometryType type0 = p0->getGeometry()->getGeometryType();
		Geometry::GeometryType type1 = p1->getGeometry()->getGeometryType();

		if(type0 > type1)
			std::swap(p0, p1);

		type0 = p0->getGeometry()->getGeometryType();
		type1 = p1->getGeometry()->getGeometryType();

		switch(type0)
		{
		case Geometry::GEOMETRY_BALL:

			switch(type1)
			{
			case Geometry::GEOMETRY_BALL:
				return new BallBallContact(p0, p1);
			case Geometry::GEOMETRY_BLOCK:
				return new BallBlockContact(p0, p1);
			}

		case Geometry::GEOMETRY_BLOCK:
			return new BlockBlockContact(p0, p1);
		default:
			RBXASSERT(0);
			return NULL;
		}
	}
}