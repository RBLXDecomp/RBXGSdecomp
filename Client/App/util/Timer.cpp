#include "util/Timer.h"
#include <windows.h>

namespace RBX
{
	double getRealTime()
	{
		LARGE_INTEGER count, frequency;
		QueryPerformanceCounter(&count);
		QueryPerformanceFrequency(&frequency);
		return (double)count.QuadPart / (double)frequency.QuadPart;
	}
}
