#include "script/LuaSignalBridge.h"
#include "script/LuaInstanceBridge.h"
#include "G3D/format.h"

using namespace RBX;
using namespace RBX::Lua;
using namespace RBX::Reflection;

template<>
int SignalBridge::on_index(const boost::shared_ptr<Reflection::SignalInstance>& object, const char* name, lua_State* L)
{
    if (!object.get())
        throw std::runtime_error(G3D::format("The %s event has been deleted", name));

    if (strcmp(name, "connect") == 0)
    {
        lua_pushcfunction(L, SignalBridge::connect);
        return 1;
    }

    if (strcmp(name, "wait") == 0)
    {
        lua_pushcfunction(L, SignalBridge::wait);
        return 1;
    }

    if (strcmp(name, "disconnect") == 0)
        throw std::runtime_error(G3D::format("Event:disconnect() has been deprecated. Use connection object returned by connect()"));

    throw std::runtime_error(G3D::format("%s is not a valid member", name));
}

template<>
void SignalBridge::on_newindex(boost::shared_ptr<Reflection::SignalInstance>& object, const char* name, lua_State* L)
{
    if (!object.get())
        throw std::runtime_error(G3D::format("The %s event has been deleted", name));

    throw std::runtime_error(G3D::format("%s cannot be assigned to", name));
}

static size_t pushArgs(lua_State* L, const Arguments& arguments)
{
    Arguments::const_iterator iter = arguments.begin();
    Arguments::const_iterator end = arguments.end();

    for (; iter != end; iter++)
    {
        const type_info& argType = iter->type();

        if (argType == typeid(std::string))
        {
            lua_pushstring(L, boost::any_cast<std::string>(*iter).c_str());
        }
        else if (argType == typeid(int))
        {
            lua_pushinteger(L, boost::any_cast<int>(*iter));
        }
        else if (argType == typeid(size_t))
        {
            lua_pushinteger(L, boost::any_cast<size_t>(*iter));
        }
        else if (argType == typeid(float))
        {
            lua_pushnumber(L, boost::any_cast<float>(*iter));
        }
        else if (argType == typeid(double))
        {
            lua_pushnumber(L, boost::any_cast<double>(*iter));
        }
        else if (argType == typeid(bool))
        {
            lua_pushboolean(L,boost::any_cast<bool>(*iter));
        }
        else if (argType == typeid(boost::shared_ptr<Instance>))
        {
            ObjectBridge::push(L, boost::any_cast<boost::shared_ptr<Instance>>(*iter));
        }
        else if (argType == typeid(const Reflection::PropertyDescriptor*))
        {
            lua_pushstring(L, boost::any_cast<const Reflection::PropertyDescriptor*>(*iter)->name.c_str());
        }
        else if (argType == typeid(const char*))
        {
            lua_pushstring(L, boost::any_cast<const char*>(*iter));
        }
        else
        {
            // ¯\_(ツ)_/¯
            // maybe they meant to print it? lol
            argType.name();
        }
    }

    return arguments.size();
}

FunctionScriptSlot::FunctionScriptSlot(lua_State* thread, int functionIndex)
    : context(ScriptContext::getContext(thread)),
      function(FunctionRef(thread, functionIndex)),
      cachedSlotThread(ThreadRef())
{
}

// TODO: 99.09% (functional match - registers swapped in codegen)
void FunctionScriptSlot::operator()(const Arguments& arguments)
{
    lua_State* mainThread = function.thread();

    if (!mainThread)
    {
        cnction->disconnect();
        return;
    }

    lua_State* newThread = cachedSlotThread.thread();
    bool createdThread = false;

    if (!newThread)
    {
        newThread = lua_newthread(mainThread);

        if (!newThread)
            throw std::runtime_error("lua_newthread failed");

        createdThread = true;
    }

    lua_pushfunction(mainThread, function);
    lua_xmove(mainThread, newThread, 1);
    int narg = static_cast<int>(pushArgs(newThread, arguments));
    ScriptContext::Result resumeResult = context.resume(newThread, narg);

    switch (resumeResult)
    {
    case ScriptContext::Yield:
        cachedSlotThread.reset();
        break;

    case ScriptContext::Error:
        cachedSlotThread.reset();
        cnction->disconnect();
        break;
    }

    if (!function.thread())
        cnction->disconnect();

    if (createdThread)
    {
        if (resumeResult == ScriptContext::Success && function.thread())
            cachedSlotThread = ThreadRef(newThread);

        lua_pop(mainThread, 1);
    }

    lua_settop(newThread, 0);
}

WaitScriptSlot::WaitScriptSlot(lua_State* thread)
    : waitThread(ThreadRef(thread))
{
}

int SignalBridge::connect(lua_State* L)
{
    boost::shared_ptr<Reflection::SignalInstance> si = getObject(L, 1);

    FunctionScriptSlot slot(L, 2);
    *(slot.cnction) = si->connectGeneric(slot, boost::signals::at_front);

    SignalConnectionBridge::pushNewObject(L, *(slot.cnction));

    return 1;
}

int SignalBridge::wait(lua_State* L)
{
    // what's with the scope? connect probably does the same thing
    {
        boost::shared_ptr<Reflection::SignalInstance> si = getObject(L, 1);

        WaitScriptSlot slot(L);
        *(slot.cnction) = si->connectGeneric(slot, boost::signals::at_front);

        RobloxExtraSpace::get(L)->yieldCaptured = true;
    }

    return lua_yield(L, 0);
}

template<>
int SignalConnectionBridge::on_index(const boost::signals::connection& object, const char* name, lua_State* L)
{
    if (strcmp(name, "disconnect") == 0)
    {
        lua_pushcfunction(L, SignalConnectionBridge::disconnect);
        return 1;
    }
    else if (strcmp(name, "connected") == 0)
    {
        lua_pushboolean(L, object.connected());
        return 1;
    }

    throw std::runtime_error(G3D::format("%s is not a valid member", name));
}

int SignalConnectionBridge::disconnect(lua_State* L)
{
    boost::signals::connection& cnction = getObject(L, 1);
    cnction.disconnect();
    return 0;
}
