#pragma once
#include "v8tree/Instance.h"
#include <G3D/GCamera.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Rect2D.h>
#include <G3D/Array.h>
#include "util/Extents.h"

namespace RBX
{
	class Primitive;
	class ContactManager;
	class ICameraSubject;
	class ICameraOwner;

	extern const char* sCamera;
	class Camera : public DescribedCreatable<Camera, Instance, &sCamera>
	{
	private:
		enum AnimationType
		{
			ALWAYS,
			AUTO
		};
	public:
		enum CameraType
		{
			FIXED_CAMERA,
			ATTACH_CAMERA,
			WATCH_CAMERA,
			TRACK_CAMERA,
			FOLLOW_CAMERA,
			CUSTOM_CAMERA,
			NUM_CAMERA_TYPE
		};
		enum ZoomType
		{
			ZOOM_IN_OR_OUT,
			ZOOM_OUT_ONLY,
			ZOOM_CHAR_PART_DRAG,
		};

	private:
		G3D::GCamera gCamera;
		G3D::CoordinateFrame cameraGoal;
		G3D::CoordinateFrame cameraFocus;
		CameraType cameraType;
		AnimationType animationType;
		boost::shared_ptr<Instance> cameraSubject;
		bool cameraExternallyAdjusted;

	private:
		ICameraOwner* getCameraOwner();
		void updateFocus();
		void updateGoal();
		bool characterZoom(float);
		bool nonCharacterZoom(float);
		void tryZoomExtents(float, float, float, const Extents&, const G3D::Rect2D&);
		ContactManager& getContactManager();
		float goalToFocusDistance() const;
		void setGCameraCoordinateFrame(const G3D::CoordinateFrame&);
		G3D::CoordinateFrame computeLineOfSiteGoal();
		void getHeadingElevationDistance(float&, float&, float&);
		void setHeadingElevationDistance(float, float, float);
		void tellCameraMoved();
		void getIgnorePrims(G3D::Array<const Primitive*>&);
		virtual bool askSetParent(const Instance*) const;
	public:
		//Camera(const Camera&);
		Camera();
		virtual ~Camera();
	public:
		const G3D::GCamera& getGCamera() const
		{
			return gCamera;
		}
		void onHeartbeat();
		void autoMode();
		void alwaysMode();
		bool isCharacterCamera() const;
		bool isFirstPersonCamera() const;
		ICameraSubject* getCameraSubject() const;
		Instance* getCameraSubjectInstance() const;
		void setCameraSubject(Instance*);
		const G3D::CoordinateFrame& getCameraFocus() const;
		void setCameraFocus(const G3D::CoordinateFrame&);
		G3D::CoordinateFrame getCameraCoordinateFrame() const
		{
			return gCamera.getCoordinateFrame();
		}
		void setCameraCoordinateFrameNoLerp(const G3D::CoordinateFrame&);
		void goalToCamera();
		CameraType getCameraType() const;
		void setCameraType(CameraType);
		bool canZoom(int) const;
		bool canTilt(int) const;
		void onWrapMouse(const G3D::Vector2&);
		bool zoom(float);
		bool setDistanceFromTarget(float);
		void zoomExtents(Extents, const G3D::Rect2D&, ZoomType);
		bool zoomExtents(const G3D::Rect2D&);
		void panRadians(float);
		void panUnits(int);
		bool tiltRadians(float);
		bool tiltUnits(int);
		void lookAt(const G3D::Vector3&);
		void setImageServerViewNoLerp(const G3D::CoordinateFrame&, const G3D::Rect2D&);
	public:
		//Camera& operator=(const Camera&);

	public:
		static float distanceDefault();
		static float distanceMin();
		static float distanceMax();
		static float distanceMaxCharacter();
		static float distanceMinOcclude();
		static float getNewZoomDistance(float, float);
	};
}
