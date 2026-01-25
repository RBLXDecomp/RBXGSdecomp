#include "script/LuaMemory.h"
#include "boost/pool/singleton_pool.hpp"
#include <algorithm>

LuaAllocator::LuaAllocator()
    : heapSize(0),
      heapCount(0),
      maxHeapSize(0),
      maxHeapCount(0)
{
}

void LuaAllocator::clearHeapMax()
{
    maxHeapSize = 0;
    maxHeapCount = 0;
}


void LuaAllocator::getHeapStats(size_t& heapSize, size_t& heapCount, size_t& maxHeapSize, size_t& maxHeapCount) const
{
    heapSize = this->heapSize;
    heapCount = this->heapCount;
    maxHeapSize = this->maxHeapSize;
    maxHeapCount = this->maxHeapCount;
}

void* LuaAllocator::alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
    return static_cast<LuaAllocator*>(ud)->alloc(ptr, osize, nsize);
}

// TODO: this
template <int a, int b>
__declspec(noinline) void* bucketRealloc(void* ptr, size_t osize, size_t nsize)
{
    return NULL;
}

void* LuaAllocator::alloc(void* ptr, size_t osize, size_t nsize)
{
    void* result = bucketRealloc<16, 32>(ptr, osize, nsize);

    heapSize += (nsize-osize);

    if (osize == 0)
        heapCount++;

    if (nsize == 0)
        heapCount--;

    maxHeapSize = std::max<size_t>(maxHeapSize, heapSize);
    maxHeapCount = std::max<size_t>(maxHeapCount, heapCount);

    return result;
}
