#include "util/Profiling.h"
#include <g3d/system.h>

namespace RBX
{
	namespace Profiling
	{
		DWORD Mark::markTlsIndex = NULL;

		void init(bool enabled)
		{
			if (enabled)
				Mark::markTlsIndex = TlsAlloc();
		}

		Bucket::Bucket()
			: sampleTimeSpan(0),
			  kernTimeSpan(0),
			  userTimeSpan(0),
			  frames(0)
		{
		}

		Profiler::Profiler(const char* name)
			: bucketTimeSpan(1.0),
			  currentBucket(0),
			  lastSampleTime(G3D::System::getTick()),
			  name(name)
		{
		}

		CodeProfiler::CodeProfiler(const char* name)
			: Profiler(name),
			  parent(NULL)
		{
		}

		ThreadProfiler::ThreadProfiler(const char* name)
			: Profiler(name),
			  initialized(false)
		{
		}

		void ThreadProfiler::sample(HANDLE thread)
		{
			double time = G3D::System::getTick();
			if (bucketTimeSpan + lastSampleTime <= time)
			{
				FILETIME creationTime;
				FILETIME exitTime;
				FILETIME kernelTime;
				FILETIME userTime;

				if (GetThreadTimes(thread, &creationTime, &exitTime, &kernelTime, &userTime))
				{
					ULARGE_INTEGER kernelTime64;
					kernelTime64.LowPart = kernelTime.dwLowDateTime;
					kernelTime64.HighPart = kernelTime.dwHighDateTime;

					ULARGE_INTEGER userTime64;
					userTime64.LowPart = userTime.dwLowDateTime;
					userTime64.HighPart = userTime.dwHighDateTime;

					if (initialized)
					{
						buckets[currentBucket].sampleTimeSpan = time - lastSampleTime;
						buckets[currentBucket].kernTimeSpan += kernelTime64.QuadPart;
						buckets[currentBucket].userTimeSpan += userTime64.QuadPart;
						currentBucket = (currentBucket + 1) & 4095;
					}
					else
					{
						initialized = true;
					}

					lastSampleTime = time;
					buckets[currentBucket].kernTimeSpan = -(G3D::int64)kernelTime64.QuadPart;
					buckets[currentBucket].userTimeSpan = -(G3D::int64)userTime64.QuadPart;
				}
			}
		}

		void CodeProfiler::log(G3D::int64 kern, G3D::int64 user, bool frameTick)
		{
			double time = G3D::System::getTick();
			if (bucketTimeSpan + lastSampleTime <= time)
			{
				buckets[currentBucket].sampleTimeSpan = time - lastSampleTime;
				currentBucket = (currentBucket + 1) & 4095;
				buckets[currentBucket].frames = frameTick ? 1 : 0;
				buckets[currentBucket].kernTimeSpan = kern;
				buckets[currentBucket].userTimeSpan = user;
				lastSampleTime = time;
			}
			else
			{
				if (frameTick)
					++buckets[currentBucket].frames;
				buckets[currentBucket].kernTimeSpan += kern;
				buckets[currentBucket].userTimeSpan += user;
			}
		}

		Bucket Profiler::getData(double window) const
		{
			// TODO
			return Bucket();
		}

		double Bucket::getActualFPS() const
		{
			return sampleTimeSpan > 0.0 ? frames / sampleTimeSpan : 0.0;
		}

		double Bucket::getNominalFPS() const
		{
			return frames / getTotalTime();
		}

		double Bucket::getFrameTime() const
		{
			return getTotalTime() / frames;
		}

		double Bucket::getTotalTime() const
		{
			return (kernTimeSpan + userTimeSpan) * 0.0000001;
		}
	}
}
