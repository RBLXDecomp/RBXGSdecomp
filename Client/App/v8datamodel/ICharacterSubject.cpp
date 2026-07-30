#include "v8datamodel/ICharacterSubject.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/UserController.h"
#include "v8datamodel/Workspace.h"
#include "v8world/ContactManager.h"
#include "v8world/Primitive.h"

namespace RBX
{
	ICharacterSubject::ICharacterSubject()
		: lastOffset(G3D::Vector3::ZERO),
		  cursorLocked(false),
		  wasOccluded(false),
		  hasGoalInFocus(false)
	{
	}

	G3D::CoordinateFrame ICharacterSubject::getFocusLookingAtGoal(const G3D::CoordinateFrame& cameraFocus, const G3D::CoordinateFrame& cameraGoal)
	{
		G3D::CoordinateFrame temp = cameraFocus;
		temp.lookAt(cameraGoal.translation);

		return temp;
	}

	G3D::CoordinateFrame ICharacterSubject::goalFromDistance(const G3D::CoordinateFrame& cameraFocus, const G3D::CoordinateFrame& cameraGoal, const float distance)
	{
		G3D::CoordinateFrame manLookCamera = getFocusLookingAtGoal(cameraFocus, cameraGoal);
		G3D::Vector3 newCameraPt = manLookCamera.lookVector() * distance + manLookCamera.translation;

		return G3D::CoordinateFrame(G3D::Matrix3::identity(), newCameraPt);
	}

	bool ICharacterSubject::zoom(const float in, G3D::CoordinateFrame& cameraGoal, G3D::CoordinateFrame& cameraFocus)
	{
		G3D::Vector3 focusToGoal = cameraGoal.translation - cameraFocus.translation;

		float currentDistance = focusToGoal.magnitude();
		float newDistance = Camera::getNewZoomDistance(currentDistance, in);

		newDistance = G3D::min(400.0f, newDistance);

		if (newDistance == currentDistance)
			return false;

		focusToGoal.y = 0.0f;
		focusToGoal.unitize();
		focusToGoal.y = newDistance * 0.03f;

		cameraGoal.translation = focusToGoal.direction() * newDistance + cameraFocus.translation;

		return true;
	}

	//98.58% match
	bool ICharacterSubject::testOcclusion(const G3D::CoordinateFrame& cameraFocus, const G3D::CoordinateFrame& cameraGoal, float& distance)
	{
		ContactManager* contactManager = getContactManager();
		if (contactManager)
		{
			std::vector<const Primitive*> ignorePrims;
			getIgnorePrims(ignorePrims);

			G3D::CoordinateFrame manLookCamera = getFocusLookingAtGoal(cameraFocus, cameraGoal);
			G3D::Vector3 pointOnMan = manLookCamera.leftVector();

			int hits = 0;
			float originalDistance = (cameraGoal.translation - cameraFocus.translation).magnitude();
			distance = originalDistance;

			for (int x = -2; x <= 2; x += 2)
			{
				G3D::Vector3 r = manLookCamera.translation + pointOnMan * (float)x;
				G3D::Ray manToCamera = G3D::Ray::fromOriginAndDirection(r, cameraGoal.translation - r);

				G3D::Vector3 hitPoint;
				if (contactManager->getHit(manToCamera, &ignorePrims, NULL, hitPoint))
				{
					hits++;
					float thisDistance = (hitPoint - r).magnitude();
					distance = G3D::min(distance, thisDistance);
				}
			}

			if (hits > 1)
			{
				distance -= 1.0f;
				distance = G3D::max(distance, 0.5f);
				distance = G3D::min(distance, 1000.0f);
				return true;
			}
			else
			{
				distance = originalDistance;
				return false;
			}
		}

		return false;
	}

	void ICharacterSubject::onHeartBeat(G3D::CoordinateFrame& cameraGoal, G3D::CoordinateFrame& cameraFocus)
	{
		if ((cameraGoal.translation - cameraFocus.translation).magnitude() < 2.75f)
		{
			Instance* thisInstance = dynamic_cast<Instance*>(this);
			if (ControllerService* controllerService = ServiceProvider::find<ControllerService>(thisInstance))
			{
				if (Workspace* workspace = ServiceProvider::find<Workspace>(thisInstance))
				{
					UserInputBase* device = controllerService->getHardwareDevice();
					if (device)
					{
						device->centerCursor();
						device->setWrapMode(UserInputBase::WRAP_CENTER);
						cursorLocked = true;
					}
				}
			}
		}
		else
		{
			Instance* thisInstance = dynamic_cast<Instance*>(this);
			if (ControllerService* controllerService = ServiceProvider::find<ControllerService>(thisInstance))
			{
				if (Workspace* workspace = ServiceProvider::find<Workspace>(thisInstance))
				{
					UserInputBase* device = controllerService->getHardwareDevice();
					if (device && cursorLocked)
					{
						if (!workspace->getInMouselookMode())
							device->setWrapMode(UserInputBase::WRAP_AUTO);

						cursorLocked = false;
					}
				}
			}
		}

		float angle = 0.0f;

		if (cursorLocked)
		{
			G3D::Vector3 v = cameraFocus.translation - cameraGoal.translation;
			v.y = 0.0f;
			v = v.direction();

			angle = Math::angleToE0(G3D::Vector2(v.z, v.x));
		}

		cameraSetWalkOrientation(angle, !cursorLocked);
	}
}
