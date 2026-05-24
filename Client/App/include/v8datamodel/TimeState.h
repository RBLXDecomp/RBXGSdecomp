#pragma once
#include <G3D/Stopwatch.h>

namespace RBX
{
	class TimeState
	{
	public:
		G3D::Stopwatch simulationStopwatch;
		G3D::Stopwatch fpsStopwatch;
		float totalVirtualTime;

	public:
		TimeState();
		void clear();
	};
}
