#pragma once
#include "lua/LuaBridge.h"
#include "reflection/reflection.h"
#include "boost/signals.hpp"

namespace RBX
{
    namespace Lua
    {
        class SignalBridge : public SharedPtrBridge<Reflection::SignalInstance>
        {
        public:
            static int connect(lua_State*);
            static int wait(lua_State*);
        };

        class SignalConnectionBridge : public Bridge<boost::signals::connection, true>
        {
        private:
            static int disconnect(lua_State*);
        };
    }
}
