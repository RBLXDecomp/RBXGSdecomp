#pragma once
#include "lua/LuaBridge.h"
#include "reflection/reflection.h"
#include "lua.h"
#include "lauxlib.h"

namespace RBX
{
    namespace Lua
    {
        void newweaktable(lua_State* L, const char* mode);

        class ObjectBridge : public SharedPtrBridge<Reflection::DescribedBase>
        {
        private:
            static const luaL_Reg classLibrary[0];
  
        public:
            static int callMemberFunction(lua_State*);
            static void registerInstanceClassLibrary(lua_State* L)
            {
                luaL_register(L, "Instance", classLibrary);
                lua_pop(L, 1);
            }
            static int newInstance(lua_State*);
            static boost::shared_ptr<RBX::Instance> getInstance(lua_State*, size_t);
        };
    }
}
