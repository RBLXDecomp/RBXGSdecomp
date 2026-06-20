#pragma once
#include "humanoid/Flying.h"

namespace RBX
{
	extern const char* sJumping;

	class Jumping : public Named<Flying, &sJumping>
	{
	private:
		bool done;
		float timeout;
		
		static const float kJumpP;
		static const float jumpVelocityGrid;

	private:
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);
		virtual void onComputeForce(const float);
		virtual bool canSleep() const
		{
			return false;
		}
	public:
		Jumping(Humanoid* humanoid);
		virtual ~Jumping();
	};
}
