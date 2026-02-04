#pragma once
#include "lua.h"
#include "boost/shared_ptr.hpp"

namespace RBX
{
    namespace Lua
    {
        template <typename T, bool unknown>
        class Bridge
        {
        protected: 
            static const char* className;
  
        public:
            static T* pushNewObject(lua_State*);
            static T& getObject(lua_State*, size_t);
            static void registerClass(lua_State*);

            template <typename Object>
            static Object* pushNewObject(lua_State*, Object);
        
        protected:
            static int on_index(lua_State*);
            static int on_index(const T& object, const char* name, lua_State* L);
            static int on_newindex(lua_State*);
            static void on_newindex(T&, const char*, lua_State*);
            static int on_tostring(lua_State*);
            static int on_tostring(const T&, lua_State*);
            static int on_gc(lua_State*);
            static int on_eq(lua_State*);
        };

        template <typename T>
        class SharedPtrBridge : protected Bridge<boost::shared_ptr<T>, false>
        {
        public:
            static void registerClass(lua_State* L)
            {
                Bridge<boost::shared_ptr<T>, false>::registerClass(L);
            }
            static void registerClassLibrary(lua_State* L)
            {
                lua_pushlightuserdata(L, push);
                newweaktable(L, "v");
                lua_rawset(L, LUA_REGISTRYINDEX);
            }
            static void push(lua_State*, boost::shared_ptr<T>);
            static boost::shared_ptr<T> getPtr(lua_State*, size_t);
        };
    }
}
