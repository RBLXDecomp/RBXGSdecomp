#pragma once
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Array.h>
#include <G3D/Ray.h>
#include <boost/weak_ptr.hpp>
#include "util/NormalId.h"

namespace RBX
{
	class PartInstance;
	class Workspace;
	class Primitive;
	class Contact;

	class RunDragger
	{
	public:
		class SnapInfo
		{
		public:
			Primitive* snap;
			NormalId surface;
			G3D::Vector3 hitWorld;
			G3D::Vector3 lastDragSnap;

			SnapInfo()
				: surface(NORM_UNDEFINED),
				  hitWorld(G3D::Vector3::inf()),
				  lastDragSnap(G3D::Vector3::inf()),
				  snap(NULL)
			{
			}

			void updateHitFromSurface(const G3D::Ray& mouseRay);
			void updateSurfaceFromHit();
			float hitOutsideExtents();
		};

	private:
		boost::weak_ptr<PartInstance> dragPart;
		boost::weak_ptr<PartInstance> snapPart;
		Workspace* workspace;
		Primitive* drag;
		G3D::Vector3 dragPointLocal;
		SnapInfo snapInfo;
		G3D::Ray mouseRay;
		NormalId dragSurface;
		G3D::Vector3 dragHitLocal;

	private:
		SnapInfo createSnapSurface(Primitive* snap, G3D::Array<NormalId>* ignore);
		bool moveDragPart();
		bool snapDragPart();
		void findSafeY();
		bool notTried(Primitive* check, const G3D::Array<Primitive*>& tried);
		bool adjacent(Primitive* p0, Primitive* p1);
		SnapInfo rayHitsPart(const G3D::Array<Primitive*>& triedSnap, bool forceAdjacent);
		SnapInfo bestProximatePart(const G3D::Array<Primitive*>& triedSnap, bool (Contact::* proximityTest)(float));
		bool fallOffEdge();
		bool fallOffPart(bool&);
		bool colliding();
		bool rayHitsCloserPart();
		bool tooCloseToCamera();
		SnapInfo findSnap(const G3D::Array<Primitive*>&);
		void findNoSnapPosition(const G3D::CoordinateFrame& original);
		void smartPtrHackPre();
		void smartPtrHackPost();
	public:
		RunDragger();
		~RunDragger();
		void init(Workspace*, boost::weak_ptr<PartInstance>, const G3D::Vector3&);
		bool snap(const G3D::Ray&);

	public:
		static void turnUpright(PartInstance* part);
		static void rotatePart(PartInstance* part);
		static void tiltPart(PartInstance* part, const G3D::CoordinateFrame& camera);
	};
}
