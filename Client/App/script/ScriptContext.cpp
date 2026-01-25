#include "script/Script.h"
#include "script/LuaMemory.h"
#include "script/LuaAtomicClasses.h"
#include "script/LuaInstanceBridge.h"
#include "script/LuaSignalBridge.h"
#include "lua/LuaBridge.h"
#include "util/standardout.h"
#include "util/Debug.h"
#include "lualib.h"

int contextCount = 0;
bool vvvvv; // what

int panic(lua_State* L)
{
    std::string message = lua_tostring(L, -1);
    RBX::StandardOut::singleton()->print(
        RBX::MESSAGE_ERROR,
        "Unprotected error in call to Lua API (%s)\n",
        message.c_str()
    );

    RBXCRASH();

    return 0;
}

namespace RBX
{
    void ScriptContext::openState()
    {
        if (!globalState)
        {
            // ???
            if (vvvvv)
            {
                lua_getstack(NULL, NULL, NULL);
                lua_getinfo(NULL, NULL, NULL);
                lua_getlocal(NULL, NULL, NULL);
                lua_setlocal(NULL, NULL, NULL);
                lua_getupvalue(NULL, NULL, NULL);
                lua_setupvalue(NULL, NULL, NULL);
                lua_sethook(NULL, NULL, NULL, NULL);
                lua_gethook(NULL);
                lua_gethookmask(NULL);
                lua_gethookcount(NULL);
            }
            
            allocator.reset(new LuaAllocator());
            lua_State* L = lua_newstate(LuaAllocator::alloc, allocator.get());
            globalState = L;

            if (!globalState)
                throw new std::runtime_error("Failed to create Lua state");

            contextCount++;

            lua_atpanic(L, panic);

            ////
            lua_pushlightuserdata(globalState, (void*)0x43);
            lua_pushlightuserdata(globalState, this);
            lua_settable(globalState, LUA_GLOBALSINDEX);
            lua_pop(globalState, luaopen_base(globalState));
            ////

            ////
            lua_pushcfunction(globalState, luaopen_string);
            lua_pushstring(globalState, "string");
            lua_call(globalState, 1, 0);
            ////

            ////
            lua_pushcfunction(globalState, luaopen_math);
            lua_pushstring(globalState, "math");
            lua_call(globalState, 1, 0);
            ////

            ////
            lua_pushcfunction(globalState, luaopen_table);
            lua_pushstring(globalState, "table");
            lua_call(globalState, 1, 0);
            ////

            Lua::ObjectBridge::registerClass(globalState);
            Lua::SignalBridge::registerClass(globalState);
            Lua::SignalConnectionBridge::registerClass(globalState);
            Lua::CoordinateFrameBridge::registerClass(globalState);
            Lua::Vector3Bridge::registerClass(globalState);
            Lua::Color3Bridge::registerClass(globalState);
            Lua::BrickColorBridge::registerClass(globalState);
            Lua::Bridge<boost::shared_ptr<Lua::ThreadRef::Node>, true>::registerClass(globalState);
            // ^^ this likely was a subclass that got optimised out due to this
            // being the only use of it

            Lua::CoordinateFrameBridge::registerClassLibrary(globalState);
            Lua::Vector3Bridge::registerClassLibrary(globalState);
            Lua::Color3Bridge::registerClassLibrary(globalState);
            Lua::BrickColorBridge::registerClassLibrary(globalState);
            Lua::ObjectBridge::registerClassLibrary(globalState);
            Lua::ObjectBridge::registerInstanceClassLibrary(globalState);
            Lua::SignalBridge::registerClassLibrary(globalState);

            Lua::ThreadRef::Node::create(globalState);

            // TODO
        }
    }
}
