#include "script/LuaInstanceBridge.h"
#include "script/LuaSignalBridge.h"
#include "script/LuaAtomicClasses.h"
#include "script/ThreadRef.h"
#include "v8tree/Instance.h"
#include "G3D/format.h"
#include "G3D/Vector3.h"

using namespace RBX;
using namespace RBX::Lua;
using namespace RBX::Reflection;

void RBX::Lua::newweaktable(lua_State* L, const char* mode)
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

// TODO: 90.17%
static void pushLuaValue(ConstProperty p, lua_State* L)
{
    // (0024BC)  S_BPREL32: [FFFFFF40], Type:             0x3639, objects
    // (0024D0)  S_BPREL32: [FFFFFF50], Type:             0x36A2, end
    // (0024E0)  S_BPREL32: [FFFFFF48], Type:             0x36A2, iter

    PropertyDescriptor& descriptor = const_cast<PropertyDescriptor&>(p.getDescriptor());

    if (descriptor.type == Type::singleton<int>())
    {
        lua_pushinteger(L, p.getValue<int>());
        return;
    }

    if (descriptor.type == Type::singleton<bool>())
    {
        lua_pushboolean(L, p.getValue<bool>());
        return;
    }

    if (descriptor.type == Type::singleton<float>())
    {
        lua_pushnumber(L, p.getValue<float>());
        return;
    }

    if (descriptor.type == Type::singleton<std::string>())
    {
        lua_pushstring(L, p.getStringValue().c_str());
        return;
    }

    if (descriptor.type == Type::singleton<boost::shared_ptr<Instance>>())
    {
        ObjectBridge::push(L, p.getValue<boost::shared_ptr<Instance>>());
        return;
    }

    if (descriptor.type == Type::singleton<boost::shared_ptr<DescribedBase>>())
    {
        ObjectBridge::push(L, p.getValue<boost::shared_ptr<DescribedBase>>());
        return;
    }

    if (descriptor.type == Type::singleton<G3D::Vector3>())
    {
        Vector3Bridge::pushVector3(L, p.getValue<G3D::Vector3>());
        return;
    }

    if (descriptor.type == Type::singleton<G3D::CoordinateFrame>())
    {
        CoordinateFrameBridge::pushCoordinateFrame(L, p.getValue<G3D::CoordinateFrame>());
        return;
    }

    if (descriptor.type == Type::singleton<G3D::Color3>())
    {
        Color3Bridge::pushColor3(L, p.getValue<G3D::Color3>());
        return;
    }

    if (descriptor.type == Type::singleton<BrickColor>())
    {
        BrickColorBridge::pushNewObject(L, p.getValue<BrickColor>());
        return;
    }

    if (descriptor.type == Type::singleton<ContentId>())
    {
        lua_pushstring(L, p.getStringValue().c_str());
        return;
    }

    if (descriptor.type == Type::singleton<FunctionRef>())
    {
        lua_pushfunction(L, p.getValue<FunctionRef>());
        return;
    }

    if (descriptor.type == Type::singleton<boost::shared_ptr<const Instances>>())
    {
        boost::shared_ptr<const Instances> objects = p.getValue<boost::shared_ptr<const Instances>>();
        if (objects)
        {
            Instances::const_iterator end = objects->end();    
            Instances::const_iterator iter = objects->begin();    

            // objects->size() doesn't match
            lua_createtable(L, end-iter, 0);

            for (int i = 1; iter != end; i++)
            {
                ObjectBridge::push(L, *iter);
                lua_rawseti(L, -2, i);
                ++iter;
            }
        }
        else
        {
            lua_newtable(L);
        }
        return;
    }

    EnumPropertyDescriptor* enumDesc = dynamic_cast<EnumPropertyDescriptor*>(&descriptor);
    if (enumDesc)
    {
        lua_pushinteger(L, enumDesc->getEnumValue(p.getInstance()));
        return;
    }

    RefPropertyDescriptor* refDesc = dynamic_cast<RefPropertyDescriptor*>(&descriptor);
    if (refDesc)
    {
        DescribedBase* base = refDesc->getRefValue(p.getInstance());

        // this isn't being optimised correctly
        if (base)
            ObjectBridge::push(L, static_cast<Instance*>(base)->shared_from_this());
        else
            ObjectBridge::push(L, boost::shared_ptr<DescribedBase>());

        return;
    }

    throw std::runtime_error(G3D::format("Unable to get property %s, type %s", descriptor.name.c_str(), descriptor.type.name.c_str()));
}

// TODO: 95.24% (functional match)
template<>
int ObjectBridge::on_index(const boost::shared_ptr<DescribedBase>& object, const char* name, lua_State* L)
{
    // (003FA0)  S_BPREL32: [FFFFFF54], Type:             0x3058, iter
    // (003FB4)  S_BPREL32: [FFFFFF54], Type:             0x316B, iter
    // (003FC8)  S_BPREL32: [FFFFFF54], Type:             0x30E9, iter
    // (003FDC)  S_BPREL32: [FFFFFF74], Type:             0x2EF7, s
    // (003FEC)  S_BPREL32: [FFFFFF54], Type:             0x13AC, name2

    if (!object)
        throw std::runtime_error("The object has been deleted");

    const Name& name2 = Name::lookup(name);

    {
        PropertyIterator iter = object->findProperty(name2);
        if (iter != object->properties_end())
        {
            pushLuaValue(*iter, L);
            return 1;
        }
    }

    {
        FunctionIterator iter = object->findFunction(name2);
        if (iter != object->functions_end())
        {
            const FunctionDescriptor* fd = (*iter).getDescriptor();

            lua_pushlightuserdata(L, (void*)fd);
            lua_rawget(L, LUA_ENVIRONINDEX);

            if (lua_type(L, -1) == LUA_TNIL)
            {
                lua_pop(L, 1);
                lua_pushlightuserdata(L, (void*)fd);
                lua_pushcclosure(L, ObjectBridge::callMemberFunction, 1);
                lua_pushlightuserdata(L, (void*)fd);
                lua_pushvalue(L, -2);
                lua_settable(L, LUA_ENVIRONINDEX);
            }

            RBXASSERT(lua_type(L, -1) == LUA_TFUNCTION);

            return 1;
        }
    }

    {
        SignalIterator iter = object->findSignal(name2);
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

    if (name2.empty())
    {
        std::string pascalName = name;
        pascalName[0] = toupper(name[0]);

        if (pascalName[0] != name[0] && !Name::lookup(pascalName).empty())
            return on_index(object, pascalName.c_str(), L);
    }

    throw std::runtime_error(G3D::format("%s is not a valid member of %s", name, instance->getClassName().c_str()));
}

// ObjectBridge::callMemberFunction depends on RBX::Security