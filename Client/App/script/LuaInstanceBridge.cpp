#include "script/LuaInstanceBridge.h"

namespace RBX
{
    namespace Lua
    {
        void newweaktable(lua_State* L, const char* mode)
        {
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            lua_pushliteral(L, "__mode");
            lua_pushstring(L, mode);
            lua_settable(L, -3);
        }
    }
}
