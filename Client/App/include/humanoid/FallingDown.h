#pragma once
#include "humanoid/Humanoid.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sFallingDown;

	class FallingDown : public Named<Humanoid::State, &sFallingDown>
	{
	private:
		float time;
		const float timeToGetUp;

	private:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);

		virtual void onComputeForce(const float dt)
		{
			return;
		}

		virtual bool canSleep() const
		{
			return false;
		}

	public:
		FallingDown(Humanoid* humanoid, float timeToGetUp);
		virtual ~FallingDown();
	};
}
