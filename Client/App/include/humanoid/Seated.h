#pragma once
#include "humanoid/Humanoid.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sSeated;

	class Seated : public Named<Humanoid::State, &sSeated>
	{
	private:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);

		virtual void onComputeForce(const float dt)
		{
			return;
		}

		virtual bool canSleep() const
		{
			return true;
		}

	public:
		Seated(Humanoid* humanoid);
		virtual ~Seated();
	};
}
