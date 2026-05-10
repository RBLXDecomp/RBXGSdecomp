#include "v8datamodel/ICharacterSubject.h"
#include "v8datamodel/Camera.h"

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
}
