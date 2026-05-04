#pragma once

class LuaAllocator
{
private:
    size_t heapSize;
    size_t heapCount;
    size_t maxHeapSize;
    size_t maxHeapCount;
    
public:
    LuaAllocator();
    void clearHeapMax();
    void getHeapStats(size_t&, size_t&) const;
    void getHeapStats(size_t& heapSize, size_t& heapCount, size_t& maxHeapSize, size_t& maxHeapCount) const;
    void* alloc(void* ptr, size_t osize, size_t nsize);
    
public: 
    static void* alloc(void* ud, void* ptr, size_t osize, size_t nsize);
};
