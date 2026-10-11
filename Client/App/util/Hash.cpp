#include "util/Hash.h"

namespace RBX
{
	unsigned int Hash::hash(const std::string& str)
	{
		return hash(reinterpret_cast<const void*>(str.data()), str.size());
	}

	unsigned int Hash::hash(const void* data, size_t size)
	{
		unsigned int hash = 0;
		hashAppend(hash, data, size);
		return hash;
	}

	void Hash::hashAppend(unsigned int& hash, const void* data, size_t size)
	{
		unsigned int a = 63689;

		for (size_t i = 0; i < size; ++i)
		{
			hash = hash * a + reinterpret_cast<const unsigned char*>(data)[i];
			a *= 378551;
		}

		hash &= ~0x80000000;
	}
}
