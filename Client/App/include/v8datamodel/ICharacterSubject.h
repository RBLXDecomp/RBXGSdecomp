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

	class ICharacterSubject : public ICameraSubject
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
		G3D::CoordinateFrame getFocusLookingAtGoal(const G3D::CoordinateFrame&, const G3D::CoordinateFrame&);
		bool testOcclusion(const G3D::CoordinateFrame&, const G3D::CoordinateFrame&, float&);
		G3D::CoordinateFrame goalFromDistance(const G3D::CoordinateFrame&, const G3D::CoordinateFrame&, const float);
		virtual bool zoom(const float, G3D::CoordinateFrame&, G3D::CoordinateFrame&);
		virtual void stepGoalAndFocus(G3D::CoordinateFrame&, G3D::CoordinateFrame&, bool);
	public:
		//ICharacterSubject(const ICharacterSubject&);
		ICharacterSubject();
	public:
		virtual ContactManager* getContactManager();
		virtual void getIgnorePrims(std::vector<const Primitive*>&);
		virtual void tellCameraNear(float);
		virtual G3D::Vector3 getIntendedMovementVector();
		virtual float getIntendedRotationAboutYAxis();
		virtual void cameraSetWalkOrientation(float, bool);
		float panAdjust();
		void onHeartBeat(G3D::CoordinateFrame&, G3D::CoordinateFrame&);
		void setCursorCenterLocked(bool);
		bool getCursorCenterLocked() const;
	public:
		virtual ~ICharacterSubject();
	public:
		//ICharacterSubject& operator=(const ICharacterSubject&);
	};
}
