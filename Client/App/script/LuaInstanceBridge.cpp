#include "script/LuaInstanceBridge.h"
#include "script/LuaSignalBridge.h"
#include "v8tree/Instance.h"
#include "G3D/format.h"

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

        int ObjectBridge::newInstance(lua_State* thread)
        {
            const Name& name = Name::lookup(lua_tostring(thread, 1));
            boost::shared_ptr<Instance> instance = AbstractFactoryProduct<Instance>::create(name);

            if (lua_gettop(thread) >= 2)
            {
                boost::shared_ptr<Instance> parent = ObjectBridge::getInstance(thread, 2);

                if (parent)
                    instance->setParent(parent.get());
            }

            ObjectBridge::push(thread, instance);

            return 1;
        }
    }
}

using namespace RBX::Reflection;

static void pushLuaValue(ConstProperty p, lua_State* L)
{
    if (p.getDescriptor().type == Type::singleton<int>())
    {
        lua_pushinteger(L, p.getValue<int>());
        return;
    }

    if (p.getDescriptor().type == Type::singleton<bool>())
    {
        lua_pushboolean(L, p.getValue<bool>());
        return;
    }

    if (p.getDescriptor().type == Type::singleton<float>())
    {
        lua_pushnumber(L, p.getValue<float>());
        return;
    }

    if (p.getDescriptor().type == Type::singleton<std::string>())
    {
        lua_pushboolean(L, p.getValue<bool>());
        return;
    }

    if (p.getDescriptor().type == Type::singleton<bool>())
    {
        lua_pushstring(L, p.getStringValue().c_str());
        return;
    }

    // TODO
}

namespace RBX
{
    namespace Lua
    {
        // TODO: 91.43% 
        // seems like it's due to the descriptor inlines from object.h -- *very* close but not quite yet
        template<>
        int ObjectBridge::on_index(const boost::shared_ptr<Reflection::DescribedBase>& object, const char* name, lua_State* L)
        {
            // (003FA0)  S_BPREL32: [FFFFFF54], Type:             0x3058, iter
            // (003FB4)  S_BPREL32: [FFFFFF54], Type:             0x316B, iter
            // (003FC8)  S_BPREL32: [FFFFFF54], Type:             0x30E9, iter
            // (003FDC)  S_BPREL32: [FFFFFF74], Type:             0x2EF7, s
            // (003FEC)  S_BPREL32: [FFFFFF54], Type:             0x13AC, name2

            if (!object.get())
                throw std::runtime_error("The object has been deleted");

            const Name& name2 = Name::lookup(name);

            // are these scopes inlines?

            {
                MemberDescriptorContainer<PropertyDescriptor>::Iterator iter = object->findProperty(name2);
                if (iter != object->properties_end())
                {
                    pushLuaValue(*iter, L);
                    return 1;
                }
            }

            {
                MemberDescriptorContainer<FunctionDescriptor>::ConstIterator iter = object->findFunction(name2);
                if (iter != object->functions_end())
                {
                    const FunctionDescriptor* fd = (*iter).getDescriptor();

                    lua_pushlightuserdata(L, (void*)fd);
                    lua_rawget(L, LUA_ENVIRONINDEX);

                    if (lua_type(L, -1) == LUA_TNIL)
                    {
                        lua_pop(L, 1);
                        lua_pushlightuserdata(L, (void *)fd);
                        lua_pushcclosure(L, ObjectBridge::callMemberFunction, 1);
                        lua_pushlightuserdata(L, (void *)fd);
                        lua_pushvalue(L, -2);
                        lua_settable(L, LUA_ENVIRONINDEX);
                    }

                    RBXASSERT(lua_type(L, -1) == LUA_TFUNCTION);

                    return 1;
                }
            }

            {
                MemberDescriptorContainer<SignalDescriptor>::Iterator iter = object->findSignal(name2);
                if (iter != object->signals_end())
                {
                    Signal s = *iter;
                    SignalBridge::push(L, s.getSignalInstance());
                    return 1;
                }
            }

            Instance* instance = dynamic_cast<Instance*>(object.get());
            if (instance)
            {
                Instance* child = instance->findFirstChildByName(name);
                if (child)
                {
                    ObjectBridge::push(L, shared_from(child));
                    return 1;
                }
            }

            if (name2 == Name::getNullName())
            {
                std::string test = name;
                test[0] = toupper(name[0]);

                if (test[0] != name[0] && !Name::lookup(test).empty())
                {
                    return on_index(object, test.c_str(), L);
                }
            }

            throw std::runtime_error(G3D::format("%s is not a valid member of %s", name, instance->getClassName().c_str()));
        }

        // ObjectBridge::callMemberFunction depends on RBX::Security
    }
}