#pragma once
#include <string>

namespace RBX
{
	namespace Render
	{
		struct SpecData
		{
		public:
			const float shaderModel;
			const int videoMemory;
			const int CPUSpeed;
			const std::string glVendor;
			const std::string gfxcard;
			const std::string cpu;
			const int ram;
			const std::string resolution;
		public:
			static bool glLoaded;

		public:
			SpecData();
		};
	}
}
