#pragma once
#include "humanoid/Balancing.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sFlying;

	class Flying : public Named<Balancing, &sFlying>
	{
	private:
		float desiredYRotationalVelocity;

		static const float turnSpeed;
		static const float kTurnP;
		static const float kTurnAccelMax;

	protected:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);
		virtual void onComputeForce(const float);

		virtual bool canSleep() const
		{
			return false;
		}

		Flying(Humanoid* humanoid);
	};
}
