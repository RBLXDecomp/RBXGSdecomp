#include "tool/RunDragger.h"
#include "tool/Dragger.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Filters.h"
#include "v8datamodel/Surfaces.h"
#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/Workspace.h"
#include "v8world/Primitive.h"
#include "v8world/Contact.h"
#include "v8world/ContactManager.h"

namespace RBX
{
	void RunDragger::SnapInfo::updateSurfaceFromHit()
	{
		G3D::Vector3 hitInSnap = snap->getCoordinateFrame().pointToObjectSpace(hitWorld);
		Extents snapExtents = snap->getExtentsLocal();

		surface = snapExtents.closestFace(hitInSnap);
	}

	void RunDragger::SnapInfo::updateHitFromSurface(const G3D::Ray& mouseRay)
	{
		Extents snapExtents = snap->getExtentsLocal();
		const G3D::CoordinateFrame& cframe = snap->getCoordinateFrame();

		G3D::Ray rayInSnap = cframe.toObjectSpace(mouseRay);
		G3D::Plane plane = snapExtents.getPlane(surface);
		G3D::Vector3 hitInSnap;

		if (Math::intersectRayPlane(rayInSnap, plane, hitInSnap))
		{
			hitWorld = cframe.pointToWorldSpace(hitInSnap);
		}
		else
		{
			RBXASSERT(0);
			Math::intersectRayPlane(rayInSnap, plane, hitInSnap);
		}
	}

	float RunDragger::SnapInfo::hitOutsideExtents()
	{
		Extents snapExtents = snap->getExtentsLocal();
		G3D::Vector3 hitInSnap = snap->getCoordinateFrame().pointToObjectSpace(hitWorld);
		G3D::Vector3 clipInSnap = hitInSnap.clamp(snapExtents.min(), snapExtents.max());

		return (hitInSnap - clipInSnap).magnitude();
	}

	RunDragger::RunDragger()
		: workspace(NULL),
		  drag(NULL)
	{
	}

	RunDragger::~RunDragger() {}

	bool RunDragger::adjacent(Primitive* p0, Primitive* p1)
	{
		for (Contact* current = p0->getFirstContact(); current != NULL; current = p0->getNextContact(current))
		{
			if (current->otherPrimitive(p0) == p1)
				return current->computeIsAdjacent(0.01f);
		}

		return false;
	}

	bool RunDragger::tooCloseToCamera()
	{
		return (drag->getCoordinateFrame().pointToWorldSpace(dragPointLocal) - mouseRay.origin).magnitude() < 6.0f;
	}

	void RunDragger::rotatePart(PartInstance* part)
	{
		RBXASSERT(part);

		G3D::CoordinateFrame rotated = part->getCoordinateFrame();
		Math::rotateMatrixAboutY90(rotated.rotation, 1);

		part->setCoordinateFrame(rotated);
	}

	RunDragger::SnapInfo RunDragger::createSnapSurface(Primitive* snap, G3D::Array<NormalId>* ignore)
	{
		RBXASSERT(snap);

		SnapInfo answer;
		answer.snap = snap;

		Extents snapExtents = snap->getExtentsLocal();
		const G3D::CoordinateFrame& cframe = snap->getCoordinateFrame();
		G3D::Ray rayInSnap = cframe.toObjectSpace(mouseRay);

		float bestDistance = 0.0f;
		bool found = false;

		for (int i = 0; i < 6; i++)
		{
			NormalId id = (NormalId)i;

			if (!ignore || ignore->find(id) == ignore->end())
			{
				G3D::Plane plane = snapExtents.getPlane(id);
				if (plane.halfSpaceContainsFinite(rayInSnap.origin))
				{
					G3D::Vector3 hitInSnap;
					if (Math::intersectRayPlane(rayInSnap, plane, hitInSnap))
					{
						G3D::Vector3 hitWorld = cframe.pointToWorldSpace(hitInSnap);
						float distance = (mouseRay.origin - hitWorld).magnitude();
						if (distance > bestDistance)
						{
							bestDistance = distance;
							answer.surface = id;
							found = true;
							answer.hitWorld = hitWorld;
						}
					}
				}
			}
		}
		
		return found ? answer : SnapInfo();
	}

	bool RunDragger::notTried(Primitive* check, const G3D::Array<Primitive*>& tried)
	{
		RBXASSERT(check != drag);
		return check && tried.find(check) == tried.end();
	}

	RunDragger::SnapInfo RunDragger::bestProximatePart(const G3D::Array<Primitive*>& triedSnap, bool (Contact::* proximityTest)(float))
	{
		SnapInfo answer;

		for (Contact* current = drag->getFirstContact(); current != NULL; current = drag->getNextContact(current))
		{
			if ((current->*proximityTest)(0.01f))
			{
				Primitive* otherPrimitive = current->otherPrimitive(drag);
				if (notTried(otherPrimitive, triedSnap))
				{
					SnapInfo testInfo = createSnapSurface(otherPrimitive, NULL);
					if (testInfo.snap && testInfo.hitOutsideExtents() < 1e+10)
						answer = testInfo;
				}
			}
		}

		return answer;
	}

	void RunDragger::turnUpright(PartInstance* part)
	{
		RBXASSERT(part);

		if (part->getSurfaces().isStandardPart())
		{
			G3D::CoordinateFrame dragCoord = part->getCoordinateFrame();
			if (Math::getClosestObjectNormalId(G3D::Vector3::unitY(), dragCoord.rotation) != NORM_Y)
			{
				dragCoord.rotation = G3D::Matrix3::identity();
				part->setCoordinateFrame(dragCoord);
			}
		}
	}

	void RunDragger::tiltPart(PartInstance* part, const G3D::CoordinateFrame& camera)
	{
		RBXASSERT(part);

		G3D::CoordinateFrame dragCoord = part->getCoordinateFrame();
		G3D::Vector3 axis = Math::getWorldNormal(Math::getClosestObjectNormalId(camera.rightVector(), dragCoord.rotation), dragCoord);
		G3D::Matrix3 tiltM = G3D::Matrix3::fromAxisAngle(axis, G3D::halfPi());

		dragCoord.rotation = tiltM * dragCoord.rotation;
		part->setCoordinateFrame(dragCoord);
	}

	RunDragger::SnapInfo RunDragger::rayHitsPart(const G3D::Array<Primitive*>& triedSnap, bool forceAdjacent)
	{
		SnapInfo answer;
		PartByLocalCharacter filter((Instance*)workspace);

		if (PartInstance* mousePart = MouseCommand::getMousePart(mouseRay, workspace->getWorld()->getContactManager(), drag, &filter, answer.hitWorld, 2048.0f))
		{
			answer.snap = mousePart->getPrimitive();
			if (notTried(answer.snap, triedSnap) && (!forceAdjacent || adjacent(answer.snap, drag)))
			{
				answer.updateSurfaceFromHit();
				return answer;
			}
		}

		return SnapInfo();
	}

	bool RunDragger::rayHitsCloserPart()
	{
		G3D::Array<Primitive*> ignoreNone;
		SnapInfo answer = rayHitsPart(ignoreNone, false);

		if (answer.snap && answer.snap != snapInfo.snap)
		{
			if ((answer.hitWorld - mouseRay.origin).magnitude() < (snapInfo.hitWorld - mouseRay.origin).magnitude())
				return true;
		}

		return false;
	}

	void RunDragger::findSafeY()
	{
		if (workspace->getWorld()->getContactManager().intersectingOthers(drag, 0.01f))
		{
			PartInstance* part = PartInstance::fromPrimitive(drag);
			G3D::Array<Primitive*> temp;
			temp.push_back(part->getPrimitive());

			G3D::Vector3 moved = Dragger::safeMoveNoDrop(temp, G3D::Vector3::zero(), workspace->getWorld()->getContactManager());
			RBXASSERT(moved.x == 0.0f);
			RBXASSERT(moved.y != 0.0f);

			G3D::CoordinateFrame newC = drag->getCoordinateFrame();
			newC.translation += moved;

			dragPart.lock()->setCoordinateFrame(newC);
		}
	}

	void RunDragger::findNoSnapPosition(const G3D::CoordinateFrame& original)
	{
		dragPart.lock()->setCoordinateFrame(original);
		findSafeY();

		RBXASSERT(!workspace->getWorld()->getContactManager().intersectingOthers(drag, 0.01f));
	}
}
