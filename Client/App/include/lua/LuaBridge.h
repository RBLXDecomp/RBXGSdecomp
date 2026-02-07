#pragma once
#include "lua.h"
#include "lauxlib.h"
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
            static T* pushNewObject(lua_State* L);
            static T& getObject(lua_State* L, size_t index)
            {
                return *static_cast<T*>(luaL_checkudata(L, static_cast<int>(index), className));
            }
            static void registerClass(lua_State*);

            template <typename Object>
            static Object* pushNewObject(lua_State* L, Object param1)
            {
                Object* ptr = static_cast<Object*>(lua_newuserdata(L, sizeof(Object)));
                new(ptr) Object(param1);
                luaL_getmetatable(L, className);
                lua_setmetatable(L, -2);
                return ptr;
            }
        
        protected:
            static int on_index(lua_State*);
            static int on_index(const T& object, const char* name, lua_State* L);
            static int on_newindex(lua_State* L)
            {
                const char* name = luaL_checkstring(L, 2);
                T* object = static_cast<T*>(luaL_checkudata(L, 1, className));
                on_newindex(*object, name, L);
                return 0;
            }
            static void on_newindex(T& object, const char* name, lua_State* L);
            static int on_tostring(lua_State* L);
            static int on_tostring(const T& object, lua_State* L);
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
            static void push(lua_State* L, boost::shared_ptr<T> instance)
            {
                // TODO: check match
                if (instance)
                {
                    lua_gettop(L);
                    lua_pushlightuserdata(L, push);
                    lua_rawget(L, LUA_REGISTRYINDEX);
                    lua_pushlightuserdata(L, instance.get());
                    lua_rawget(L, -2);

                    if (lua_type(L, -1) == LUA_TNIL)
                    {
                        lua_pop(L, 1);
                        pushNewObject(L, instance);
                        lua_pushlightuserdata(L, instance.get());
                        lua_pushvalue(L, -2);
                        lua_rawset(L, -4);
                    }

                    lua_remove(L, -2);
                }
                else
                {
                    lua_pushnil(L);
                }
            }
            static boost::shared_ptr<T> getPtr(lua_State*, size_t);
        };
    }
}
