#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
#include <G3D/Array.h>
#include <vector>
#include "util/Extents.h"

namespace RBX
{
	class Primitive;
	class ContactManager;
	class PVInstance;

	class Dragger
	{
	private:
		static void primitivesFromInstances(const std::vector<PVInstance*>&, G3D::Array<Primitive*>&);
		static bool intersectingWorldOrOthers(const G3D::Array<Primitive*>&, ContactManager&);
		static void movePrimitives(const G3D::Array<Primitive*>&, const G3D::Vector3&);
		static void movePrimitivesDelta(const G3D::Array<Primitive*>&, const G3D::Vector3&, G3D::Vector3&);
		static void movePrimitivesGoal(const G3D::Array<Primitive*>&, const G3D::Vector3&, G3D::Vector3&);
		static void safePlaceAlongLine(const G3D::Array<Primitive*>&, const G3D::Vector3&, const G3D::Vector3&, G3D::Vector3&, ContactManager&);
		static void searchUpFine(const G3D::Array<Primitive*>&, G3D::Vector3&, ContactManager&);
		static void searchDownFine(const G3D::Array<Primitive*>&, G3D::Vector3&, ContactManager&);
		static void searchUpGross(const G3D::Array<Primitive*>&, G3D::Vector3&, ContactManager&);
		static void searchDownGross(const G3D::Array<Primitive*>&, G3D::Vector3&, ContactManager&);
	public:
		static const G3D::Vector3& dragSnap();
		static G3D::Vector3 toGrid(const G3D::Vector3&);
		static G3D::Vector3 safeMoveNoDrop(const G3D::Array<Primitive*>&, const G3D::Vector3&, ContactManager&);
		static G3D::Vector3 safeMoveYDrop(const G3D::Array<Primitive*>&, const G3D::Vector3&, ContactManager&);
		static G3D::Vector3 safeMoveAlongLine(const G3D::Array<Primitive*>&, const G3D::Vector3&, ContactManager&);
		static void safeRotate(const G3D::Array<Primitive*>&, const G3D::Matrix3&, ContactManager&);
		static Extents computeExtents(const std::vector<PVInstance*>&);
		static Extents computeExtents(const G3D::Array<Primitive*>&);
		static Extents computeExtents(const std::vector<Primitive*>&);
	};
}
