#include "RenderLib/Profiler.h"
#include <GLG3D/GLCaps.h>
#include <GLG3D/DXCaps.h>
#include <G3D/RegistryUtil.h>
#include <boost/lexical_cast.hpp>

namespace RBX
{
	namespace Render
	{
		/* 
			Part of the following code found here was not originally written by Roblox. Functions nearly identical to the ones seen here 
			have been found in the Kitware Standard Library (KWSys) and were decompiled using code from this library as a reference.

			KWSys (Copyright 2000-2016 Kitware, Inc. and contributors, all rights reserved) is released under the 
			BSD 3-clause license here: https://gitlab.kitware.com/utils/kwsys

			For license details, see the following link: https://gitlab.kitware.com/utils/kwsys/-/blob/master/Copyright.txt
		*/

		static float getShaderModel()
		{
			if ((G3D::GLCaps::supports_GL_ARB_fragment_shader() && G3D::GLCaps::supports_GL_ARB_vertex_shader()) ||
				(G3D::GLCaps::supports_GL_ARB_shader_objects() && G3D::GLCaps::supports_GL_ARB_shading_language_100()))
				return 2.0f;

			if (G3D::GLCaps::supports("GL_ATI_fragment_shader") || G3D::GLCaps::supports("GL_NV_fragement_program")) // yes, this is a typo. crazy, right?
				return 2.0f;

			if (G3D::GLCaps::supports("GL_ATI_texture_env_combine3") || G3D::GLCaps::supports("GL_NV_texture_shader3"))
				return 1.4f;

			if (G3D::GLCaps::supports("GL_NV_register_combiners"))
				return 1.2f;

			return 1.0f;
		}

		typedef void (*DELAY_FUNC)(unsigned uiMS);

		static G3D::int64 GetCyclesDifference(DELAY_FUNC DelayFunction, unsigned uiParameter)
		{
			unsigned t0Hi, t0Lo;
			unsigned t1Hi, t1Lo;

			// Calculate the frequency of the CPU instructions.
			__try
			{
				__asm
				{
					rdtsc
					mov esi, eax
					mov edi, edx

					push uiParameter
					call DelayFunction
					
					pop ebx

					rdtsc
					mov t1Hi, edx
					mov t1Lo, eax
					mov t0Hi, edi
					mov t0Lo, esi
				}
			}
			__except (1)
			{
				return 0;
			}

			return (((G3D::int64)t1Hi << 32) + t1Lo) - (((G3D::int64)t0Hi << 32) + t0Lo);
		}

		void Delay(unsigned time)
		{
			LARGE_INTEGER Frequency, StartCounter, EndCounter;
			G3D::int64 x;

			// Get the frequency of the high performance counter.
			if (!QueryPerformanceFrequency(&Frequency)) 
				return;

			x = Frequency.QuadPart / 1000 * time;

			// Get the starting position of the counter.
			QueryPerformanceCounter(&StartCounter);

			do 
			{
				// Get the ending position of the counter.  
				QueryPerformanceCounter(&EndCounter);
			} 
			while (EndCounter.QuadPart - StartCounter.QuadPart < x);
		}

		void DelayOverhead(unsigned time)
		{
			LARGE_INTEGER Frequency, StartCounter, EndCounter;
			G3D::int64 x;

			// Get the frequency of the high performance counter.
			if (!QueryPerformanceFrequency(&Frequency)) 
				return;

			x = Frequency.QuadPart / 1000 * time;

			// Get the starting position of the counter.
			QueryPerformanceCounter(&StartCounter);
		  
			do 
			{
				// Get the ending position of the counter.  
				QueryPerformanceCounter(&EndCounter);
			} 
			while (EndCounter.QuadPart - StartCounter.QuadPart == x);
		}

		static int getCPUSpeed()
		{
			G3D::int32 speed;

			// TODO: wrong version of RegistryUtil
			// this is actually RegistryUtil::read32bitNumber, the function was renamed to readInt32 in a later G3D commit
			if (G3D::RegistryUtil::readInt32("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\~MHz", speed))
			{
				if (speed > 300)
					return speed;
			}

			unsigned repetitions = 2;
			unsigned mSecPerRepetition = 50;
			G3D::int64 total = 0;
			G3D::int64 overhead = 0;

			for (int i = 0; i < 2; i++)
			{
				total += GetCyclesDifference(&Delay, 50);
				overhead += GetCyclesDifference(&DelayOverhead, 50);
			}

			// Calculate the MHz speed.
			total -= overhead;
			total /= repetitions;
			total /= mSecPerRepetition;
			total /= 1000;

			if (total < 100 || total > 50000)
				total = 1400;

			return total;
		}

		static std::string getCPUMake()
		{
			std::string cpu_name;
			if (G3D::RegistryUtil::readString("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\ProcessorNameString", cpu_name))
				return cpu_name;
			else
				return "Could not determine CPU name";
		}

		static std::string getMaxRes()
		{
			int width = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);

			std::string w = boost::lexical_cast<std::string>(width);
			std::string h = boost::lexical_cast<std::string>(height);

			return w + "x" + h;
		}

		static int inline_getTotalRAM()
		{
			MEMORYSTATUS memoryStatus;
			GlobalMemoryStatus(&memoryStatus);

			return memoryStatus.dwTotalPhys;
		}

		SpecData::SpecData()
			: shaderModel(glLoaded ? getShaderModel() : 0.0f),
			  videoMemory(G3D::DXCaps::videoMemorySize() / (1 << 20)),
			  glVendor(glLoaded ? G3D::GLCaps::vendor() : "?"),
			  gfxcard(glLoaded ? G3D::GLCaps::renderer() : "?"),
			  CPUSpeed(getCPUSpeed()),
			  cpu(getCPUMake()),
			  ram(inline_getTotalRAM() / (1 << 20)),
			  resolution(getMaxRes())
		{
		}
	}
}
