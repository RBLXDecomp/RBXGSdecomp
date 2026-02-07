#pragma once
#include "lua/LuaBridge.h"
#include "script/Script.h"
#include "script/ScriptContext.h"
#include "script/ThreadRef.h"
#include "reflection/reflection.h"
#include "boost/signals.hpp"

namespace RBX
{
    namespace Lua
    {
        class SignalBridge : public SharedPtrBridge<Reflection::SignalInstance>
        {
        public:
            static int connect(lua_State* L);
            static int wait(lua_State* L);
        };

        class SignalConnectionBridge : public Bridge<boost::signals::connection, true>
        {
            // wha
            friend class Bridge<boost::signals::connection, true>;

        private:
            static int disconnect(lua_State* L);
        };
    }
}

class FunctionScriptSlot : public RBX::Script::Slot
{
private:
    RBX::ScriptContext& context;
    RBX::Lua::FunctionRef function;
    RBX::Lua::ThreadRef cachedSlotThread;
  
public:
    FunctionScriptSlot(lua_State* thread, int functionIndex);
    void operator()(const RBX::Reflection::Arguments&);
};

class WaitScriptSlot : public RBX::Script::Slot
{
private:
    RBX::Lua::ThreadRef waitThread;
  
public:
    WaitScriptSlot(lua_State* thread);
    void operator()(const RBX::Reflection::Arguments&);
};
