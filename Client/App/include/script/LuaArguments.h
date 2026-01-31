#pragma once
#include "reflection/reflection.h"
#include "lua.h"

namespace RBX
{
    namespace Lua
    {
        typedef std::vector<Reflection::Value> ArgList;

        class LuaArguments : public Reflection::FunctionDescriptor::Arguments 
        {
        private:
            const int offset;
            lua_State* L;
  
        public:
            LuaArguments(lua_State*, int);
            virtual size_t size() const;
            virtual void get(int, Reflection::Value&) const;
            int push(const Reflection::Value&) const;
            int pushReturnValue() const;

        public:
            static void get(lua_State*, int, Reflection::Value&);
            static int push(const Reflection::Value&, lua_State*);
        };
    }
}
