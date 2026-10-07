#pragma once
#include <G3D/Vector3.h>
#include <G3D/System.h>
#include <boost/noncopyable.hpp>
#include <string>

namespace RBX
{
	namespace Profiling
	{
		void init(bool enabled);

		struct Bucket
		{
		public:
			double sampleTimeSpan;
			G3D::int64 kernTimeSpan;
			G3D::int64 userTimeSpan;
			int frames;

		public:
			double getActualFPS() const;
			double getNominalFPS() const;
			double getFrameTime() const;
			double getTotalTime() const;
		public:
			Bucket();
		public:
			Bucket& operator+=(const Bucket& other);
		};

		class Profiler : public boost::noncopyable
		{
		protected:
			const double bucketTimeSpan;
			int currentBucket;
			Bucket buckets[4096];
			G3D::RealTime lastSampleTime;
		public:
			const std::string name;

		public:
			Profiler(const char* name);
		public:
			Bucket getData(double window) const;
		};

		class ThreadProfiler : public Profiler
		{
		private:
			bool initialized;
		  
		public:
			ThreadProfiler(const char* name);
		public:
			void sample(HANDLE thread);
		};

		class CodeProfiler : public Profiler
		{
			friend class Mark;

		public:
			CodeProfiler *parent;

		public:
			CodeProfiler(const char* name);
		private:
			void log(G3D::int64 kern, G3D::int64 user, bool frameTick);
		};

		class Mark
		{
		private:
			CodeProfiler& section;
			CodeProfiler* enclosingSection;
			FILETIME creationTime;
			FILETIME exitTime;
			FILETIME kernelTime;
			FILETIME userTime;
			bool frameTick;

		public: 
			static DWORD markTlsIndex;

			Mark(CodeProfiler& sectionSet, bool frameTickSet)
				: section(sectionSet),
				  frameTick(frameTickSet)
			{
				if (markTlsIndex)
				{
					enclosingSection = (CodeProfiler*)TlsGetValue(markTlsIndex);
					GetThreadTimes(GetCurrentThread(), &creationTime, &exitTime, &kernelTime, &userTime);
					TlsSetValue(markTlsIndex, (void*)&sectionSet);
				}
			}

			~Mark()
			{
				FILETIME currUserTime;
				FILETIME currKernelTime;
				if (markTlsIndex)
				{
					ULARGE_INTEGER currKernelTime64, currUserTime64, kernelTime64, userTime64;
					G3D::int64 kernelTimeDelta, userTimeDelta;

					TlsSetValue(markTlsIndex, enclosingSection);
						
					GetThreadTimes(GetCurrentThread(), &creationTime, &exitTime, &currKernelTime, &currUserTime);

					kernelTime64.LowPart = kernelTime.dwLowDateTime;
					kernelTime64.HighPart = kernelTime.dwHighDateTime;
					userTime64.LowPart = userTime.dwLowDateTime;
					userTime64.HighPart = userTime.dwHighDateTime;

					currKernelTime64.LowPart = currKernelTime.dwLowDateTime;
					currKernelTime64.HighPart = currKernelTime.dwHighDateTime;
					currUserTime64.LowPart = currUserTime.dwLowDateTime;
					currUserTime64.HighPart = currUserTime.dwHighDateTime;

					kernelTimeDelta = (currKernelTime64.QuadPart - kernelTime64.QuadPart);
					userTimeDelta = (currUserTime64.QuadPart - userTime64.QuadPart);

					section.log(kernelTimeDelta, userTimeDelta, frameTick);

					if (enclosingSection && enclosingSection != &section && enclosingSection != section.parent)
					{
						enclosingSection->log(-kernelTimeDelta, -userTimeDelta, false);
					}
				}
			}
		};
	}
}