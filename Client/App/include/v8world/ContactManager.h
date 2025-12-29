#pragma once
#include <set>
#include <vector>
#include <G3D/Vector3.h>
#include <G3D/Ray.h>
#include <G3D/Array.h>
#include "util/HitTestFilter.h"
#include "util/Extents.h"

namespace RBX
{
	class Contact;
	class Primitive;
	class World;
	class SpatialHash;

	class ContactManager
	{
	private:
		SpatialHash* spatialHash;
		World* world;
	private:
		static bool ignoreBool;

	private:
		Contact* createContact(Primitive* p0, Primitive* p1);
		void stepBroadPhase();
		Primitive* getSlowHit(const G3D::Array<Primitive*>& primitives, const G3D::Ray& unitRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, float maxDistance, bool& inside, bool& stopped) const;
		Primitive* getFastHit(const G3D::Ray& worldRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPointWorld, bool& inside, bool& stopped) const;
	public:
		ContactManager(World* world);
		~ContactManager();
	public:
		const SpatialHash& getSpatialHash()
		{
			return *spatialHash;
		}

		Primitive* getHit(const G3D::Ray& worldRay, const std::vector<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, bool& inside) const;
		Primitive* getHit(const G3D::Ray& worldRay, const G3D::Array<Primitive const*>* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPoint, bool& inside) const;
		void getPrimitivesTouchingExtents(const Extents& extents, const Primitive* ignore, G3D::Array<Primitive*>& found);
		bool intersectingOthers(Primitive* check, const std::set<Primitive*>& checkSet, float overlapIgnored);
		bool intersectingOthers(const G3D::Array<Primitive*>& check, float overlapIgnored);
		bool intersectingOthers(Primitive* check, float overlapIgnored);
		void onNewPair(Primitive* p0, Primitive* p1);
		void onReleasePair(Primitive* p0, Primitive* p1);
		void onPrimitiveAdded(Primitive* p);
		void onPrimitiveRemoved(Primitive* p);
		void onPrimitiveExtentsChanged(Primitive* p);
		void onPrimitiveGeometryTypeChanged(Primitive* p);
		void stepWorld();
		RBX::Primitive* getHitLegacy(const G3D::Ray& originDirection, const Primitive* ignorePrim, const HitTestFilter* filter, G3D::Vector3& hitPointWorld, float& distanceToHit, const float& maxSearchDepth) const;
	};
}
