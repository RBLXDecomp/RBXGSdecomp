#pragma once
#include "v8kernel/Constants.h"

namespace RBX
{
	class KernelInput
	{
	private:
		float lastGoal;
		float currentGoal;
		float increment;
		bool latchToZero;
	private:
		// NOTE: this is a guess
		const int steps()
		{
			return Constants::kernelStepsPerUiStep();
		}
	public:
		KernelInput() : lastGoal(0), currentGoal(0), latchToZero(false), increment(0) {}

		// NOTE: this is a guess
		void setGoal(float goal)
		{
			if (goal == this->lastGoal)
			{
				this->currentGoal = goal;
				this->increment = 0.0f;
			}
			else
			{
				this->lastGoal = goal;
				this->increment = (goal - this->currentGoal) / steps();
			}
		}

		// NOTE: this is a guess
		void setDelta(float value)
		{
			this->increment = value;
			if (value == 0.0f)
				this->latchToZero = true;
		}

		// NOTE: this is a guess
		void get(float _lastGoal, float& _currentGoal, float& _lastGoal)
		{
			if (this->latchToZero)
			{
				this->latchToZero = false;
				this->currentGoal += goal;
				this->currentGoal *= 0.5f;
			}
			_lastGoal = this->currentGoal;
			this->currentGoal += this->increment;
			_currentGoal = this->currentGoal;
		}
	};
}
