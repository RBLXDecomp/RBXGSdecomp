#include "v8datamodel/Camera.h"
#include "v8datamodel/Workspace.h"
#include "util/Math.h"

namespace RBX
{
	const char* sCamera = "Camera";

	static const Reflection::EnumPropDescriptor<Camera, Camera::CameraType> desc_cameraType("CameraType", "Camera", &Camera::getCameraType, &Camera::setCameraType, Reflection::PropertyDescriptor::STANDARD); 
	static const Reflection::PropDescriptor<Camera, G3D::CoordinateFrame> desc_CoordFrame("CoordinateFrame", "Data", &Camera::getCameraCoordinateFrame, &Camera::setCameraCoordinateFrameNoLerp, Reflection::PropertyDescriptor::STREAMING);
	static const Reflection::PropDescriptor<Camera, G3D::CoordinateFrame> desc_Focus("Focus", "Data", &Camera::getCameraFocus, &Camera::setCameraFocus, Reflection::PropertyDescriptor::STREAMING);
	static const Reflection::RefPropDescriptor<Camera, Instance> cameraSubjectProp("CameraSubject", "Camera", &Camera::getCameraSubjectInstance, &Camera::setCameraSubject, Reflection::PropertyDescriptor::STANDARD);

	Camera::Camera()
		: Base(),
		  cameraFocus(G3D::Vector3(0, 0, -5)),
		  cameraType(FIXED_CAMERA),
		  animationType(AUTO),
		  cameraExternallyAdjusted(false)
	{
		setName("Camera");
		gCamera.setNearPlaneZ(1.25);
		gCamera.setFarPlaneZ(5000);
		gCamera.setFieldOfView(G3D::toRadians(60));

		G3D::CoordinateFrame cameraCoord(G3D::Vector3(0, 5, 5));
		cameraCoord.lookAt(G3D::Vector3::zero());

		if (Math::legalCameraCoord(cameraCoord))
		{
			gCamera.setCoordinateFrame(cameraCoord);
		}
		else
		{
			RBXASSERT(0);
		}
	}

	bool Camera::askSetParent(const Instance *instance) const
	{
		return fastDynamicCast<const Workspace>(instance) != NULL;
	}

	ICameraOwner* Camera::getCameraOwner()
	{
		Instance* instance = getParent();
		while (instance != NULL)
		{
			ICameraOwner* cameraOwner = fastDynamicCast<ICameraOwner>(instance);
			if (cameraOwner)
				return cameraOwner;

			instance = getParent();
		}

		return NULL;
	}

	void Camera::lookAt(const G3D::Vector3 &point)
	{
		cameraFocus.translation = point;
		cameraGoal.lookAt(point);
	}

	void Camera::getHeadingElevationDistance(float &heading, float &elevation, float &distance)
	{
		Math::getHeadingElevation(cameraGoal, heading, elevation);
		distance = (cameraGoal.translation - cameraFocus.translation).magnitude();
	}

	void Camera::alwaysMode()
	{
		animationType = ALWAYS;
	}

	bool Camera::nonCharacterZoom(float in)
	{
		G3D::Vector3 lookVector = cameraFocus.translation - cameraGoal.translation;

		float currentDistance = lookVector.magnitude();
		float newZoomDistance = getNewZoomDistance(currentDistance, in);

		if (currentDistance == newZoomDistance)
			return false;

		cameraGoal.translation -= lookVector * (newZoomDistance / currentDistance - 1.0f);

		ICameraOwner *owner = getCameraOwner();
		if (owner)
			owner->cameraMoved();

		return true;
	}

	void Camera::panRadians(float angle)
	{
		RBXASSERT(angle > -100);
		RBXASSERT(angle < 100);

		if (angle != 0)
		{
			float heading, elevation, distance;

			getHeadingElevationDistance(heading, elevation, distance);
			heading = Math::radWrap(heading + angle);
			setHeadingElevationDistance(heading, elevation, distance);

			ICameraOwner *owner = getCameraOwner();
			if (owner)
				owner->cameraMoved();
		}
	}

	void Camera::updateFocus()
	{
		ICameraSubject* cameraSubject = getCameraSubject();
		cameraFocus = cameraSubject->getLocation();
	}

	bool Camera::zoom(float in)
	{
		if (cameraType == CUSTOM_CAMERA)
		{
			ICameraSubject* cameraSubject = getCameraSubject();
			if (cameraSubject)
				return cameraSubject->zoom(in, cameraGoal, cameraFocus);
		}
		else if (getCameraSubjectInstance() && 
				 (cameraType == FOLLOW_CAMERA || 
				  cameraType == ATTACH_CAMERA || 
				  cameraType == TRACK_CAMERA))
		{
			return characterZoom(in);
		}
		else
		{
			return nonCharacterZoom(in);
		}

		return false;
	}

	Instance* Camera::getCameraSubjectInstance() const
	{
		return cameraSubject.get();
	}

	ICameraSubject* Camera::getCameraSubject() const
	{
		Instance* instance = getCameraSubjectInstance();
		if (instance)
		{
			ICameraSubject* subject = fastDynamicCast<ICameraSubject>(instance);
			RBXASSERT(subject);
			return subject;
		}
		return NULL;
	}

	void Camera::autoMode()
	{
		if (animationType != AUTO)
		{
			animationType = AUTO;

			ICameraOwner *owner = getCameraOwner();
			if (owner)
				owner->cameraMoved();
		}
	}

	void Camera::setCameraType(CameraType type)
	{
		if (cameraType != type)
		{
			cameraType = type;
			raisePropertyChanged(desc_cameraType);

			ICameraOwner *owner = getCameraOwner();
			if (owner)
				owner->cameraMoved();
		}
	}

	void Camera::setCameraFocus(const G3D::CoordinateFrame &value)
	{
		if (value != cameraFocus)
		{
			cameraFocus = value;
			raisePropertyChanged(desc_Focus);

			ICameraOwner *owner = getCameraOwner();
			if (owner)
				owner->cameraMoved();
		}
	}

	void Camera::goalToCamera()
	{
		if (gCamera.getCoordinateFrame() != cameraGoal)
		{
			cameraExternallyAdjusted = true;
			if (Math::legalCameraCoord(cameraGoal))
			{
				gCamera.setCoordinateFrame(cameraGoal);
			}
			else
			{
				RBXASSERT(0);
			}
			raisePropertyChanged(desc_CoordFrame);

			ICameraOwner *owner = getCameraOwner();
			if (owner)
				owner->cameraMoved();
		}
	}

	bool Camera::zoomExtents(const G3D::Rect2D &viewPort)
	{
		ICameraOwner *owner = getCameraOwner();
		if (owner)
		{
			zoomExtents(owner->computeCameraOwnerExtents(), viewPort, ZOOM_IN_OR_OUT);
			return true;
		}
		return false;
	}

	void Camera::setCameraCoordinateFrameNoLerp(const G3D::CoordinateFrame &value)
	{
		cameraGoal = value;
		goalToCamera();
	}
}