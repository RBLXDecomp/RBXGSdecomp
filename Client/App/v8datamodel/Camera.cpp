#include "v8datamodel/Camera.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/ICharacterSubject.h"
#include "util/Math.h"

namespace RBX
{
	const char* sCamera = "Camera";

	static Reflection::EnumPropDescriptor<Camera, Camera::CameraType> desc_cameraType("CameraType", "Camera", &Camera::getCameraType, &Camera::setCameraType, Reflection::PropertyDescriptor::STANDARD); 
	static Reflection::PropDescriptor<Camera, G3D::CoordinateFrame> desc_CoordFrame("CoordinateFrame", "Data", &Camera::getCameraCoordinateFrame, &Camera::setCameraCoordinateFrameNoLerp, Reflection::PropertyDescriptor::STREAMING);
	static Reflection::PropDescriptor<Camera, G3D::CoordinateFrame> desc_Focus("Focus", "Data", &Camera::getCameraFocus, &Camera::setCameraFocus, Reflection::PropertyDescriptor::STREAMING);
	static Reflection::RefPropDescriptor<Camera, Instance> cameraSubjectProp("CameraSubject", "Camera", &Camera::getCameraSubjectInstance, &Camera::setCameraSubject, Reflection::PropertyDescriptor::STANDARD);
	
	Reflection::EnumDesc<Camera::CameraType>::EnumDesc()
		: EnumDescriptor("CameraType", typeid(Camera::CameraType))
	{
		addPair(Camera::FIXED_CAMERA, "Fixed");
		addPair(Camera::WATCH_CAMERA, "Watch");
		addPair(Camera::ATTACH_CAMERA, "Attach");
		addPair(Camera::TRACK_CAMERA, "Track");
		addPair(Camera::FOLLOW_CAMERA, "Follow");
		addPair(Camera::CUSTOM_CAMERA, "Custom");
	}

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

		setGCameraCoordinateFrame(cameraCoord);
	}

	Camera::~Camera()
	{
	}

	void Camera::tellCameraMoved()
	{
		if (ICameraOwner* owner = getCameraOwner())
		{
			owner->cameraMoved();
		}
	}

	void Camera::setGCameraCoordinateFrame(const G3D::CoordinateFrame& coord)
	{
		if (Math::legalCameraCoord(coord))
		{
			gCamera.setCoordinateFrame(coord);
		}
		else
		{
			RBXASSERT(false);
		}
	}

	bool Camera::askSetParent(const Instance* instance) const
	{
		return dynamic_cast<const Workspace*>(instance) != NULL;
	}

	ICameraOwner* Camera::getCameraOwner()
	{
		for (Instance* instance = getParent(); instance != NULL; instance = instance->getParent())
		{
			ICameraOwner* cameraOwner = dynamic_cast<ICameraOwner*>(instance);
			if (cameraOwner)
				return cameraOwner;
		}

		return NULL;
	}

	void Camera::lookAt(const G3D::Vector3& point)
	{
		cameraFocus.translation = point;
		cameraGoal.lookAt(point);
	}

	void Camera::getHeadingElevationDistance(float& heading, float& elevation, float& distance)
	{
		Math::getHeadingElevation(cameraGoal, heading, elevation);
		distance = goalToFocusDistance();
	}

	bool Camera::setDistanceFromTarget(float newDistance)
	{
		G3D::Vector3 lookVector = cameraFocus.translation - cameraGoal.translation;
		float currentDistance = lookVector.magnitude();

		const float min = distanceMin();
		const float max = distanceMax();

		if (newDistance < min && currentDistance == min)
			return false;
		
		if (newDistance > max && currentDistance == max)
			return false;

		newDistance = G3D::min(max, G3D::max(min, newDistance));
		
		lookVector *= newDistance;
		cameraGoal.translation = cameraFocus.translation - (lookVector / currentDistance);

		tellCameraMoved();

		return true;
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

		if (newZoomDistance == currentDistance)
			return false;

		cameraGoal.translation -= lookVector * (newZoomDistance / currentDistance - 1.0f);

		tellCameraMoved();

		return true;
	}

	bool Camera::characterZoom(float in)
	{
		G3D::Vector3 focusToGoal = cameraGoal.translation - cameraFocus.translation;

		float currentDistance = focusToGoal.magnitude();

		float maxDistance = distanceMaxCharacter();
		float newDistance = getNewZoomDistance(currentDistance, in);
		newDistance = G3D::min(newDistance, maxDistance);

		if (newDistance == currentDistance)
			return false;

		focusToGoal.y = 0.0;
		focusToGoal.unitize();
		focusToGoal.y = newDistance * 0.03f;

		cameraGoal.translation = focusToGoal.direction() * newDistance + cameraFocus.translation;

		return true;
	}

	void Camera::setHeadingElevationDistance(float heading, float elevation, float distance)
	{
		Math::setHeadingElevation(cameraGoal, heading, elevation);
		cameraFocus.rotation = cameraGoal.rotation;

		cameraGoal.translation = cameraFocus.translation - cameraGoal.lookVector() * distance;
		cameraExternallyAdjusted = true;
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

			tellCameraMoved();
		}
	}

	bool Camera::tiltRadians(float angle)
	{
		if (angle != 0)
		{
			float heading, elevation, distance;
			getHeadingElevationDistance(heading, elevation, distance);

			static float rad = G3D::toRadians(81);
			float newElevation = G3D::clamp(elevation + angle, -rad, rad);

			if (elevation != newElevation)
			{
				setHeadingElevationDistance(heading, newElevation, distance);
				tellCameraMoved();
				return true;
			}
		}

		return false;
	}

	void Camera::updateFocus()
	{
		Instance* instance = getCameraSubjectInstance();
		if (instance)
		{
			ICameraSubject* subject = dynamic_cast<ICameraSubject*>(instance);
			RBXASSERT(subject);
			cameraFocus = subject->getLocation();
		}
	}

	void Camera::updateGoal()
	{
		switch (cameraType)
		{
			case WATCH_CAMERA:
			{
				updateFocus();
				break;
			}
			case ATTACH_CAMERA: 
			{
				G3D::Vector3 delta = cameraGoal.translation - cameraFocus.translation;
				float distance = delta.xz().length();

				updateFocus();

				G3D::Vector2 direction = -cameraFocus.lookVector().xz().direction() * distance;
				cameraGoal.translation = cameraFocus.translation + G3D::Vector3(direction.x, delta.y, direction.y);

				break;
			}
			case TRACK_CAMERA:
			{
				G3D::Vector3 oldFocusPt = cameraFocus.translation;
				updateFocus();

				cameraGoal.translation += cameraFocus.translation - oldFocusPt;
				break;
			}
			case FOLLOW_CAMERA:
			{
				G3D::Vector3 delta = cameraFocus.translation - cameraGoal.translation;
				float distance = delta.xz().length();

				updateFocus();
			
				G3D::Vector2 direction = (cameraFocus.translation.xz() - cameraGoal.translation.xz()).direction() * distance;
				cameraGoal.translation = cameraFocus.translation - G3D::Vector3(direction.x, delta.y, direction.y);

				break;
			}
			case CUSTOM_CAMERA:
			{
				if (ICameraSubject* cameraSubject = getCameraSubject())
					cameraSubject->stepGoalAndFocus(cameraGoal, cameraFocus, cameraExternallyAdjusted);

				cameraExternallyAdjusted = false;
				break;
			}
		}

		cameraGoal.lookAt(cameraFocus.translation);
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

	void Camera::onHeartbeat()
	{
		updateGoal();
		G3D::CoordinateFrame adjustedGoal = cameraGoal;
		ICameraSubject* cameraSubject = getCameraSubject();
		ICharacterSubject* characterSubject = dynamic_cast<ICharacterSubject*>(cameraSubject);

		if (characterSubject)
			characterSubject->onHeartBeat(cameraGoal, cameraFocus);

		G3D::CoordinateFrame cameraCoord = gCamera.getCoordinateFrame();
		G3D::CoordinateFrame LerpFrame = cameraCoord.lerp(adjustedGoal, 0.9f);

		setGCameraCoordinateFrame(LerpFrame);

		if (animationType == ALWAYS || !Math::fuzzyEq(LerpFrame, cameraCoord, 0.01f, 0.01f))
		{
			tellCameraMoved();
		}
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
			ICameraSubject* subject = dynamic_cast<ICameraSubject*>(instance);
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

			tellCameraMoved();
		}
	}

	void Camera::setCameraType(CameraType type)
	{
		if (cameraType != type)
		{
			cameraType = type;
			raisePropertyChanged(desc_cameraType);

			tellCameraMoved();
		}
	}

	void Camera::setCameraSubject(Instance* newSubject)
	{
		if (newSubject != getCameraSubjectInstance())
		{
			if (dynamic_cast<ICameraSubject*>(newSubject))
			{
				cameraSubject = shared_from((ModelInstance*) newSubject);
				raisePropertyChanged(cameraSubjectProp);

				tellCameraMoved();
			}
		}
	}

	void Camera::setCameraFocus(const G3D::CoordinateFrame& value)
	{
		if (value != cameraFocus)
		{
			cameraFocus = value;
			raisePropertyChanged(desc_Focus);

			tellCameraMoved();
		}
	}

	void Camera::goalToCamera()
	{
		if (gCamera.getCoordinateFrame() != cameraGoal)
		{
			cameraExternallyAdjusted = true;
			setGCameraCoordinateFrame(cameraGoal);
			raisePropertyChanged(desc_CoordFrame);

			tellCameraMoved();
		}
	}

	void Camera::tryZoomExtents(float low, float current, float high, const Extents& extents, const G3D::Rect2D& viewPort)
	{
		RBXASSERT(current >= low);
		RBXASSERT(current <= high);

		if (high - low < 0.1)
			return;

		setDistanceFromTarget(current);
		updateGoal();
		goalToCamera();

		if (extents.containedByFrustum(gCamera.frustum(viewPort)))
		{
			tryZoomExtents(low, (low + current) * 0.5, current, extents, viewPort);
		}
		else
		{
			tryZoomExtents(current, (current + high) * 0.5, high, extents, viewPort);
		}
	}

	void Camera::zoomExtents(Extents extents, const G3D::Rect2D& viewPort, Camera::ZoomType zoomType)
	{
		G3D::CoordinateFrame currentCoord = gCamera.getCoordinateFrame();
		extents.expand(0.1f);

		if (zoomType != ZOOM_CHAR_PART_DRAG || cameraType == CUSTOM_CAMERA)
		{
			if (cameraType == FIXED_CAMERA)
			{
				G3D::Vector3 scaler = extents.center() - cameraFocus.translation;

				cameraFocus.translation += scaler;
				cameraGoal.translation += scaler;
			}

			float low;

			if (zoomType == ZOOM_OUT_ONLY || zoomType == ZOOM_CHAR_PART_DRAG)
				low = goalToFocusDistance();
			else
				low = distanceMin();

			float current = goalToFocusDistance();

			RBXASSERT(G3D::isFinite(current));

			if (zoomType == ZOOM_CHAR_PART_DRAG)
				extents.scale(1.1f);

			if (G3D::isFinite(low) && G3D::isFinite(current) && G3D::isFinite(distanceMax()))
				tryZoomExtents(low, current, distanceMax(), extents, viewPort);

			cameraGoal = gCamera.getCoordinateFrame();
			setGCameraCoordinateFrame(currentCoord);
			tellCameraMoved();
		}
	}

	bool Camera::zoomExtents(const G3D::Rect2D& viewPort)
	{
		if (ICameraOwner* owner = getCameraOwner())
		{
			zoomExtents(owner->computeCameraOwnerExtents(), viewPort, ZOOM_IN_OR_OUT);
			return true;
		}
		return false;
	}

	void Camera::setCameraCoordinateFrameNoLerp(const G3D::CoordinateFrame& value)
	{
		cameraGoal = value;
		goalToCamera();
	}

	void Camera::setImageServerViewNoLerp(const G3D::CoordinateFrame& modelCoord, const G3D::Rect2D& viewPort)
	{
		G3D::Vector3 look = modelCoord.lookVector();

		bool noTilt = (fabs(look.y) > 0.95);
		if (noTilt)
		{
			look = -G3D::Vector3::unitZ();
		}
		else
		{
			look.y = 0;
			look = look.direction();
		}

		G3D::CoordinateFrame lookCoord;

		lookCoord.lookAt(look);
		lookCoord.rotation *= G3D::Matrix3::fromEulerAnglesZXY(G3D::toRadians(45), G3D::toRadians(35), 0.0f);

		look = lookCoord.lookVector();
		lookCoord.translation = (look * 10.0f) + modelCoord.translation;

		lookCoord.lookAt(modelCoord.translation);

		setCameraType(FIXED_CAMERA);
		setCameraFocus(modelCoord.translation);
		setCameraCoordinateFrameNoLerp(lookCoord);
		zoomExtents(viewPort);
		goalToCamera();
	}

	float Camera::getNewZoomDistance(float currentDistance, float in)
	{
		float const ZOOM_FACTOR = 0.25f;

		if (in > 0.0f)
		{
			currentDistance = G3D::max<float>(currentDistance / (1.0 + in * ZOOM_FACTOR), distanceMin());
		} 	
		else if (in < 0.0f)
		{
			return G3D::min<float>(currentDistance * (1.0 - in * ZOOM_FACTOR), distanceMax());
		}

		return currentDistance;
	}
}
