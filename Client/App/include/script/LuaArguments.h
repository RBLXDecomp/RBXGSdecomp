#pragma once
#include "reflection/reflection.h"
#include "script/ThreadRef.h"
#include "script/LuaAtomicClasses.h"
#include "script/LuaInstanceBridge.h"
#include "lua.h"

namespace RBX
{
    namespace Lua
    {
        class LuaArguments : public Reflection::FunctionDescriptor::Arguments 
        {
        private:
            const int offset;
            lua_State* L;
  
        public:
            LuaArguments(lua_State*, int);
            virtual size_t size() const
            {
                // TODO: check match
                return lua_gettop(L)-1;
            }
            virtual void get(int index, Reflection::Value& value) const
            {
                // TODO: check match
                int pos = index + offset;
                RBXASSERT(pos > 0);
                get(L, pos, value);
            }
            int push(const Reflection::Value&) const;
            int pushReturnValue() const;

        public:
            // TODO: 97.00%
            static void get(lua_State* L, int luaIndex, Reflection::Value& value)
            {
                // (018310)  S_BPREL32: [FFFFFF80], Type:             0x11DC, n ; const int
                // (018320)  S_BPREL32: [FFFFFF84], Type:             0x928D, values ; *std::vector<RBX::Reflection::Value> / *ValueCollection

                if (luaIndex <= lua_gettop(L))
                {
                    switch (lua_type(L, luaIndex))
                    {
                    case LUA_TNUMBER:
                    {
                        float rhs = lua_tonumber(L, luaIndex);
                        value = rhs;
                        return;
                    }

                    case LUA_TBOOLEAN:
                    {
                        bool rhs = lua_toboolean(L, luaIndex) != 0;
                        value = rhs;
                        return;
                    }

                    case LUA_TSTRING:
                    {
                        const char* str = lua_tostring(L, luaIndex);
                        value = std::string(str);
                        return;
                    }

                    case LUA_TTABLE:
                    {
                        const int n = static_cast<const int>(lua_objlen(L, luaIndex));
                        value = Reflection::ValueCollection(n);
                        Reflection::ValueCollection& values = value.cast<Reflection::ValueCollection&>();

                        if (n >= 1)
                        {
                            int i = 1;
                            do
                            {
                                Reflection::Value& ival = values[i-1];
                                lua_rawgeti(L, luaIndex, i);
                                LuaArguments::get(L, -1, ival);
                                lua_pop(L, 1);
                                i++;
                            } while (i <= n);
                        }

                        return;
                    }

                    case LUA_TNIL:
                        value = Reflection::Value();
                        break;

                    case LUA_TFUNCTION:
                        value = lua_tofunction(L, luaIndex);
                        return;

                    case LUA_TUSERDATA:
                        if (CoordinateFrameBridge::getValue(L, luaIndex, value)
                            || Vector3Bridge::getValue(L, luaIndex, value)
                            || Color3Bridge::getValue(L, luaIndex, value)
                            || BrickColorBridge::getValue(L, luaIndex, value)
                            || ObjectBridge::getPtr(L, luaIndex, value))
                            return;

                        value = Reflection::Value();
                        break;

                    default:
                        value = Reflection::Value();
                        break;
                    }
                }
            }

            static int push(const Reflection::Value& value, lua_State* const L)
            {
                if (value.isType<void>())
                    return 0;

                if (value.isType<int>())
                {
                    lua_pushinteger(L, value.cast<int>());
                    return 1;
                }

                if (value.isType<bool>())
                {
                    lua_pushboolean(L, value.cast<bool>());
                    return 1;
                }

                if (value.isType<float>())
                {
                    lua_pushnumber(L, value.cast<float>());
                    return 1;
                }

                if (value.isType<double>())
                {
                    lua_pushnumber(L, value.cast<double>());
                    return 1;
                }

                if (value.isType<FunctionRef>())
                {
                    lua_pushfunction(L, value.cast<const FunctionRef&>());
                    return 1;
                }

                if (value.isType<std::string>())
                {
                    lua_pushstring(L, value.cast<const std::string&>().c_str());
                    return 1;
                }

                if (value.isType<G3D::Vector3>())
                {
                    Vector3Bridge::pushVector3(L, value.cast<const G3D::Vector3&>());
                    return 1;
                }

                if (value.isType<G3D::CoordinateFrame>())
                {
                    CoordinateFrameBridge::pushCoordinateFrame(L, value.cast<const G3D::CoordinateFrame&>());
                    return 1;
                }

                if (value.isType<G3D::Color3>())
                {
                    Color3Bridge::pushColor3(L, value.cast<const G3D::Color3&>());
                    return 1;
                }

                if (value.isType<BrickColor>())
                {
                    BrickColorBridge::pushNewObject(L, value.cast<const BrickColor&>());
                    return 1;
                }

                if (value.isType<ContentId>())
                {
                    lua_pushstring(L, value.cast<const ContentId&>().c_str());
                    return 1;
                }

                if (value.isType<boost::shared_ptr<Instance>>())
                {
                    ObjectBridge::push(L, value.cast<const boost::shared_ptr<Instance>&>());
                    return 1;
                }

                if (value.isType<boost::shared_ptr<Reflection::DescribedBase>>())
                {
                    ObjectBridge::push(L, value.cast<const boost::shared_ptr<Reflection::DescribedBase>&>());
                    return 1;
                }

                if (value.isType<Reflection::ValueCollection>())
                {
                    const Reflection::ValueCollection& collection = value.cast<const Reflection::ValueCollection&>();
                    return pushTable(collection.begin(), collection.end(), L);
                }

                if (value.isType<boost::shared_ptr<Instances>>())
                {
                    boost::shared_ptr<Instances> values = value.cast<boost::shared_ptr<Instances>>();

                    if (values)
                    {
                        return pushTable(values->begin(), values->end(), L);
                    }
                    else
                    {
                        lua_newtable(L);
                        return 1;
                    }
                }

                return 0;
            }
        
        public:
            template <typename T>
            static int pushTable(T _First, T _Last, lua_State* const L)
            {
                lua_createtable(L, _Last-_First, 0);

                int i = 0;
                for (; _First != _Last; _First++)
                {
                    push(*_First, L);
                    lua_rawseti(L, -2, ++i);
                }

                return 1;
            }
        };
    }
}
