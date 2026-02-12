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

template <int a, int b>
void* bucketRealloc(void* ptr, size_t osize, size_t nsize)
{
    void* result;

    if (nsize > b)
    {
        if (osize <= b && osize)
        {
            result = malloc(nsize);
            memcpy(result, ptr, osize);

            if (osize > a)
                boost::singleton_pool<LuaAllocator, b>::free(ptr);
            else
                boost::singleton_pool<LuaAllocator, a>::free(ptr);
        }
        else
        {
            result = realloc(ptr, nsize);
        }
    }
    else
    {
        if (nsize > a)
        {
            if (osize > b)
            {
                result = boost::singleton_pool<LuaAllocator, b>::malloc();
                memcpy(result, ptr, nsize);
                free(ptr);
            }
            else
            {
                if (osize > a)
                {
                    result = ptr;
                }
                else
                {
                    result = boost::singleton_pool<LuaAllocator, b>::malloc();

                    if (osize > 0)
                    {
                        memcpy(result, ptr, osize);
                        boost::singleton_pool<LuaAllocator, a>::free(ptr);
                    }
                }
            }
        }
        else
        {
            if (nsize > 0)
            {
                if (osize > b)
                {
                    result = boost::singleton_pool<LuaAllocator, a>::malloc();
                    memcpy(result, ptr, nsize);
                    free(ptr);
                }
                else if (osize > a)
                {
                    result = boost::singleton_pool<LuaAllocator, a>::malloc();
                    memcpy(result, ptr, nsize);
                    boost::singleton_pool<LuaAllocator, b>::free(ptr);
                }
                else if (osize > 0)
                {
                    result = ptr;
                }
                else 
                {
                    result = boost::singleton_pool<LuaAllocator, a>::malloc();
                }
            }
            else 
            {
                if (osize > b)
                {
                    free(ptr);
                }
                else if (osize > a)
                {
                    boost::singleton_pool<LuaAllocator, b>::free(ptr);
                }
                else if (osize > 0)
                {
                    boost::singleton_pool<LuaAllocator, a>::free(ptr);
                }

                result = 0;
            }
        }
    }

    return result;
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
