#pragma once
#include "lua.h"

namespace RBX
{
    namespace Lua
    {
        class ScopedPopper 
        {
        private:
            int popCount;
            lua_State* thread;
        
        public:
            ScopedPopper(lua_State* thread, int popCount)
                : popCount(popCount),
                  thread(thread)
            {
            }
            ScopedPopper& operator+=(int);
            ScopedPopper& operator-=(int);
            ~ScopedPopper()
            {
                lua_pop(thread, popCount);
            }
        };
    }
}
