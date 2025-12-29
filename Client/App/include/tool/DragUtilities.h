#pragma once
#include <G3D/Array.h>
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Ray.h>
#include "util/Extents.h"
#include <boost/weak_ptr.hpp>
#include <vector>

namespace RBX
{
	class Primitive;
	class ContactManager;
	class World;
	class Instance;
	class PVInstance;
	class PartInstance;

	class DragUtilities
	{
	private:
		static const G3D::Array<const Primitive*>* toConstPrimitives(const G3D::Array<Primitive*>*);
	public:
		static bool notJoined(const std::vector<boost::weak_ptr<PartInstance>>&);
		static bool notJoinedToOutsiders(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void unJoinFromOutsiders(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void joinToOutsiders(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void unJoin(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void join(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void unJoinAndSetDragging(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void joinAndStopDragging(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void clean(PartInstance*);
		static void clean(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void move(const std::vector<boost::weak_ptr<PartInstance>>&, G3D::CoordinateFrame, G3D::CoordinateFrame);
		static void alignToGrid(PartInstance*);
		static void moveAndClean(PartInstance*, const G3D::Vector3&);
		static void pvsToParts(const std::vector<PVInstance*>&, std::vector<boost::weak_ptr<PartInstance>>&);
		static void instancesToParts(const std::vector<Instance*>&, std::vector<boost::weak_ptr<PartInstance>>&);
		static World* partsToPrimitives(const std::vector<boost::weak_ptr<PartInstance>>&, G3D::Array<Primitive*>&);
		static Extents computeExtents(const std::vector<boost::weak_ptr<PartInstance>>&);
		static bool hitObjectOrPlane(const ContactManager&, const G3D::Ray&, const G3D::Array<Primitive*>*, G3D::Vector3&);
		static bool anyPartAlive(const std::vector<boost::weak_ptr<PartInstance>>&);
		static void getPrimitives2(boost::shared_ptr<Instance>, std::vector<Primitive*>&);
		static void getPrimitives(const Instance*, std::vector<Primitive*>&);
		static void getPrimitivesConst(const Instance*, std::vector<const Primitive*>&);
	};
}
