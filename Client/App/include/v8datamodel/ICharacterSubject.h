#pragma once
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include <vector>
#include "util/ICameraSubject.h"
#include "util/ExponentialRunningAverage.h"

namespace RBX
{
	class Primitive;
	class ContactManager;

	class __declspec(novtable) ICharacterSubject : public ICameraSubject
	{
	private:
		Vector3ERA focusERA;
		Vector3ERA focusOffsetERA;
		G3D::Vector3 lastOffset;
		floatERA rotationERA;
		bool cursorLocked;
		bool wasOccluded;
		float unoccludedDistance;
		bool hasGoalInFocus;
		G3D::Vector3 goalInFocus;

	private:
		G3D::CoordinateFrame getFocusLookingAtGoal(const G3D::CoordinateFrame& cameraFocus, const G3D::CoordinateFrame& cameraGoal);
		bool testOcclusion(const G3D::CoordinateFrame&, const G3D::CoordinateFrame&, float&);
		G3D::CoordinateFrame goalFromDistance(const G3D::CoordinateFrame& cameraFocus, const G3D::CoordinateFrame& cameraGoal, const float distance);
		virtual bool zoom(const float in, G3D::CoordinateFrame& cameraGoal, G3D::CoordinateFrame& cameraFocus);
		virtual void stepGoalAndFocus(G3D::CoordinateFrame&, G3D::CoordinateFrame&, bool);
	public:
		ICharacterSubject();
	public:
		virtual ContactManager* getContactManager() = 0;
		virtual void getIgnorePrims(std::vector<const Primitive*>&);
		virtual void tellCameraNear(float);
		virtual G3D::Vector3 getIntendedMovementVector() = 0;
		virtual float getIntendedRotationAboutYAxis() = 0;
		virtual void cameraSetWalkOrientation(float, bool) = 0;
		float panAdjust();
		void onHeartBeat(G3D::CoordinateFrame&, G3D::CoordinateFrame&);
		void setCursorCenterLocked(bool);
		bool getCursorCenterLocked() const;
	};
}
