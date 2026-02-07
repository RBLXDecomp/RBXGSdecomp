#include "script/LuaInstanceBridge.h"
#include "script/LuaSignalBridge.h"
#include "script/LuaAtomicClasses.h"
#include "script/ThreadRef.h"
#include "util/Sound.h"
#include "util/TextureId.h"
#include "util/standardout.h"
#include "v8tree/Instance.h"
#include "G3D/format.h"
#include "G3D/Vector3.h"

// matches generally aren't very good here

using namespace RBX;
using namespace RBX::Lua;
using namespace RBX::Reflection;
using namespace RBX::Soundscape;

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

// TODO: 98.80% (functional match)
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
            Instances::const_iterator iter = objects->begin();    
            Instances::const_iterator end = objects->end();    

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
        DescribedBase* ref = refDesc->getRefValue(p.getInstance());

        // this just seems strange /shrug
        if (ref)
            ObjectBridge::push(L, static_cast<Instance*>(ref)->shared_from_this());
        else
            ObjectBridge::push(L, boost::shared_ptr<Instance>());

        return;
    }

    throw std::runtime_error(G3D::format("Unable to get property %s, type %s", descriptor.name.c_str(), descriptor.type.name.c_str()));
}

// TODO: 95.24% (functional match)
// likely due to DescribedBase inlines
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

    // what's with the scopes? they can't be inlines

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

// TODO: 94.38% (functional match)
static void assignLuaValue(Property p, lua_State* L, int index)
{
    // (002DA8)  S_BPREL32: [FFFFFF60], Type:             0x238B, value

    PropertyDescriptor& descriptor = const_cast<PropertyDescriptor&>(p.getDescriptor());

    if (descriptor.type == Type::singleton<int>())
    {
        p.setValue<int>(lua_tointeger(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<bool>())
    {
        p.setValue<bool>(lua_toboolean(L, index) != 0);
        return;
    }

    if (descriptor.type == Type::singleton<float>())
    {
        p.setValue<float>(lua_tonumber(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<std::string>())
    {
        p.setValue<std::string>(lua_tostring(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<FunctionRef>())
    {
        p.setValue(FunctionRef(L, index));
        return;

    }
    if (descriptor.type == Type::singleton<boost::shared_ptr<Instance>>())
    {
        p.setValue(ObjectBridge::getInstance(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<boost::shared_ptr<DescribedBase>>())
    {
        p.setValue(ObjectBridge::getPtr(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<G3D::Vector3>())
    {
        p.setValue(Vector3Bridge::getObject(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<G3D::CoordinateFrame>())
    {
        p.setValue(CoordinateFrameBridge::getObject(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<G3D::Color3>())
    {
        p.setValue(Color3Bridge::getObject(L, index));
        return;
    }

    if (descriptor.type == Type::singleton<BrickColor>())
    {
        p.setValue(BrickColorBridge::getObject(L, index));
        return;
    }

    // the contentid dtors for these two checks should be merged
    // seems like an inline, but i can't get it to work
    if (dynamic_cast<TypedPropertyDescriptor<SoundId>*>(&descriptor))
    {
        p.setValue<SoundId>(ContentId(lua_tostring(L, index)));
        return;
    }

    if (dynamic_cast<TypedPropertyDescriptor<TextureId>*>(&descriptor))
    {
        p.setValue<TextureId>(ContentId(lua_tostring(L, index)));
        return;
    }

    EnumPropertyDescriptor* enumDesc = dynamic_cast<EnumPropertyDescriptor*>(&descriptor);
    if (enumDesc)
    {
        int value = lua_tointeger(L, index);
        enumDesc->setEnumValue(p.getInstance(), value);
        return;
    }

    RefPropertyDescriptor* refDesc = dynamic_cast<RefPropertyDescriptor*>(&descriptor);
    if (refDesc)
    {
        boost::shared_ptr<Instance> value = ObjectBridge::getInstance(L, index);
        refDesc->setRefValue(p.getInstance(), value.get());
        return;
    }

    throw std::runtime_error(G3D::format("Unable to set property %s, type %s", descriptor.name.c_str(), descriptor.type.name.c_str()));
}

// TODO: 91.56%
// a lot of stack space gets wasted for some reason
template <>
void ObjectBridge::on_newindex(boost::shared_ptr<DescribedBase>& object, const char* name, lua_State* L)
{
    // (0033F8)  S_BPREL32: [FFFFFF68], Type:             0x3058, iter
    // (00340C)  S_BPREL32: [FFFFFF58], Type:             0x238B, parent (boost::shared_ptr<RBX::Instance>)

    if (!object)
        throw std::runtime_error("The object has been deleted");

    DescribedBase* objectPtr = object.get();
    PropertyIterator iter = object->findProperty(Name::lookup(name));
    if (iter != object->properties_end())
    {
        if ((*iter).getDescriptor() == Instance::propParent)
        {
            Instance* instance = dynamic_cast<Instance*>(objectPtr);

            if (!instance)
                throw std::runtime_error(G3D::format("%s is not a valid member of %s", name, object->classDescriptor().name.c_str()));

            boost::shared_ptr<Instance> parent = ObjectBridge::getInstance(L, 3);

            if (instance->getParent() != parent.get())
            {
                if (!instance->canSetParent(parent.get()))
                    StandardOut::singleton()->print(MESSAGE_WARNING, "%s should not be a child of %s", instance->getName().c_str(), parent->getName().c_str());

                instance->setParent2(parent);
            }
        }
        else
        {
            assignLuaValue(*iter, L, 3);    
        }

        return;
    }

    throw std::runtime_error(G3D::format("%s is not a valid member of %s", name, object->classDescriptor().name.c_str()));
}

// ObjectBridge::callMemberFunction depends on RBX::Security