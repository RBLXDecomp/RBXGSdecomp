#pragma once
#include <string>

namespace RBX
{
	class Hash
	{
	public:
		static unsigned int hash(const std::string& str);
		static unsigned int hash(const void* data, size_t size);
		static void hashAppend(unsigned int&, unsigned int);
		static void hashAppend(unsigned int& hash, const void* data, size_t size);
	};
}
