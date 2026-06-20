#pragma once
#include "humanoid/Balancing.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sRunning;

	class Running : public Named<Balancing, &sRunning>
	{
	private:
		G3D::Vector3 floorVelocity;
		float floorRotationalVelocity;
		float jumpWarmup;
		G3D::Vector3 desiredVelocity;
		float desiredYRotationalVelocity;
		float desiredAltitude;
		float intendedYRotation;

		static const float manualTurnSpeed;
		static const float autoTurnSpeed;
		static const float kTurnP;
		static const float kTurnAccelMax;
		static const float kMoveP;
		static const float kAltitudeP;
		static const float kAltitudeD;
		static const G3D::Vector3 maxMoveAccelerationGrid;

	private:
		void rotateWithGround(Body*);
		void hoverOnFloor(Body*);
		void move(Body*);
		bool isFacingStairs();
		void updateGoals(Controller&);
		virtual void onComputeForce(const float);
		virtual Humanoid::State* onStep(const float, Controller&);

		virtual bool canSleep() const
		{
			return false;
		}

		virtual float getIntendedRotationAboutYAxis() const
		{
			return intendedYRotation;
		}

	public:
		Running(Humanoid* humanoid);
		virtual ~Running();
	};
}
