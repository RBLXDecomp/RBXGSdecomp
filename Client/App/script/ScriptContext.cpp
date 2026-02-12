#include "script/ScriptContext.h"
#include "script/Script.h"
#include "script/ScriptEvent.h"
#include "script/LuaMemory.h"
#include "script/LuaAtomicClasses.h"
#include "script/LuaInstanceBridge.h"
#include "script/LuaSignalBridge.h"
#include "script/LuaArguments.h"
#include "lua/lua.hpp"
#include "lua/LuaBridge.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/DebugSettings.h"
#include "v8datamodel/Stats.h"
#include "util/standardout.h"
#include "util/Debug.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "lua.h"
#include "lualib.h"
#include "lstate.h"

static int contextCount = 0;
bool vvvvv;

using namespace RBX;
using namespace boost::posix_time;

// unidentified inlines
static inline void _openLibInline(lua_State* L, lua_CFunction f, const char* name)
{
    lua_pushcfunction(L, f);
    lua_pushstring(L, name);
    lua_call(L, 1, 0);
}

static inline void _disassociateScriptInline(Script* script)
{
    Association<Instance>& assoc = script->association();

    if (assoc.contains<Lua::ThreadRef::NodePtr>())
    {
        assoc.get<Lua::ThreadRef::NodePtr>()->eraseAllRefs();
        assoc.remove<Lua::ThreadRef::NodePtr>();
    }
}

static inline Instance* _findDataModelInline(ScriptContext* sc)
{
    Instance* parent = sc->getParent();

    if (parent)
        return parent->getRootAncestor();
    else
        return sc;
}

template<typename Class, typename Event>
static inline void _addListenerInline(RunService* runService, Listener<Class, Event>* listener)
{
    if (runService)
        runService->Notifier<Class, Event>::addListener(listener);
}

template<typename Class, typename Event>
static inline void _removeListenerInline(RunService* runService, Listener<Class, Event>* listener)
{
    if (runService)
        runService->Notifier<Class, Event>::removeListener(listener);
}

static inline void _removeListenersInline(boost::shared_ptr<RunService>& runService, ScriptContext* scriptContext)
{
    _removeListenerInline<RunService, RunTransition>(runService.get(), scriptContext);
    _removeListenerInline<RunService, Heartbeat>(runService.get(), scriptContext);
}
// end unidentified inlines

Reflection::BoundProp<bool, true> ScriptContext::propScriptsDisabled("ScriptsDisabled", "State", &ScriptContext::scriptsDisabled, &ScriptContext::onChangedScriptEnabled, Reflection::PropertyDescriptor::LEGACY); 

static int panic(lua_State* L)
{
    std::string message = lua_tostring(L, -1);
    StandardOut::singleton()->print(MESSAGE_ERROR, "Unprotected error in call to Lua API (%s)\n", message.c_str());
    RBXCRASH();
    return 0;
}

const char* RBX::sScriptContext = "ScriptContext";

void ScriptContext::openState()
{
    if (!globalState)
    {
        // ???
        if (vvvvv)
        {
            lua_getstack(NULL, NULL, NULL);
            lua_getinfo(NULL, NULL, NULL);
            lua_getlocal(NULL, NULL, NULL);
            lua_setlocal(NULL, NULL, NULL);
            lua_getupvalue(NULL, NULL, NULL);
            lua_setupvalue(NULL, NULL, NULL);
            lua_sethook(NULL, NULL, NULL, NULL);
            lua_gethook(NULL);
            lua_gethookmask(NULL);
            lua_gethookcount(NULL);
        }

        allocator.reset(new LuaAllocator());
        lua_State* L = lua_newstate(LuaAllocator::alloc, allocator.get());
        globalState = L;

        if (!globalState)
            throw std::runtime_error("Failed to create Lua state");

        contextCount++;

        lua_atpanic(L, panic);

        lua_pushlightuserdata(globalState, RBX_LUA_GLOBAL_SCRIPTCONTEXT);
        lua_pushlightuserdata(globalState, this);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pop(globalState, luaopen_base(globalState));

        _openLibInline(globalState, luaopen_string, LUA_STRLIBNAME);
        _openLibInline(globalState, luaopen_math, LUA_MATHLIBNAME);
        _openLibInline(globalState, luaopen_table, LUA_TABLIBNAME);

        Lua::ObjectBridge::registerClass(globalState);
        Lua::SignalBridge::registerClass(globalState);
        Lua::SignalConnectionBridge::registerClass(globalState);
        Lua::CoordinateFrameBridge::registerClass(globalState);
        Lua::Vector3Bridge::registerClass(globalState);
        Lua::Color3Bridge::registerClass(globalState);
        Lua::BrickColorBridge::registerClass(globalState);
        Lua::ThreadRef::NodeBridge::registerClass(globalState);

        Lua::CoordinateFrameBridge::registerClassLibrary(globalState);
        Lua::Vector3Bridge::registerClassLibrary(globalState);
        Lua::Color3Bridge::registerClassLibrary(globalState);
        Lua::BrickColorBridge::registerClassLibrary(globalState);
        Lua::ObjectBridge::registerClassLibrary(globalState);
        Lua::ObjectBridge::registerInstanceClassLibrary(globalState);
        Lua::SignalBridge::registerClassLibrary(globalState);

        Lua::ThreadRef::Node::create(globalState);

        lua_pushstring(globalState, "game");
        Lua::ObjectBridge::push(globalState, _findDataModelInline(this)->shared_from_this());
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "workspace");
        Lua::ObjectBridge::push(globalState, ServiceProvider::find<Workspace>(this)->shared_from_this());
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "shared");
        lua_newtable(globalState);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "print");
        lua_pushcfunction(globalState, ScriptContext::print);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "tick");
        lua_pushcfunction(globalState, ScriptContext::tick);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "wait");
        lua_pushcfunction(globalState, ScriptContext::wait);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "delay");
        lua_pushcfunction(globalState, ScriptContext::delay);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "trustedThread");
        lua_pushcfunction(globalState, ScriptContext::trustedThread);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "dofile");
        lua_pushcfunction(globalState, ScriptContext::dofile);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "loadfile");
        lua_pushcfunction(globalState, ScriptContext::loadfile);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        lua_pushstring(globalState, "stats");
        lua_pushcfunction(globalState, ScriptContext::stats);
        lua_settable(globalState, LUA_GLOBALSINDEX);

        yieldEvent.reset(new Lua::YieldingThreads(this));
    }
}

// TODO: 99.79% (functional match)
// stack offsets not the same
void ScriptContext::closeState()
{
    if (globalState)
    {
        commandLineSandbox.reset();

        try
        {
            Lua::ThreadRef::Node::get(globalState)->eraseAllRefs();

            std::set<Script*> copy = scripts;
            std::for_each(copy.begin(), copy.end(), boost::bind(&ScriptContext::disassociateState, this, _1));

            yieldEvent.reset(NULL);
        }
        catch (std::exception& e)
        {
            StandardOut::singleton()->print(MESSAGE_ERROR, "Exception thrown while cleaning up Lua: %s", e.what()); 
        }

        lua_close(globalState);
        contextCount--;

        size_t heapSize, heapCount, maxHeapSize, maxHeapCount;
        allocator->getHeapStats(heapSize, heapCount, maxHeapSize, maxHeapCount);
        StandardOut::singleton()->print(MESSAGE_INFO, "Script Heap Stats: Max Size = %d bytes, Max Count = %d blocks", maxHeapSize, maxHeapCount);

        allocator->clearHeapMax();

        if (contextCount == 0 && (heapSize > 0 || heapCount > 0))
            StandardOut::singleton()->print(MESSAGE_ERROR, "Script memory leaks: %d bytes, %d blocks", heapSize, heapCount);

        globalState = NULL;
    }
}

ScriptContext::ScriptContext()
    : globalState(NULL),
      nextPendingScripts(second_clock::local_time()),
      scriptsDisabled(false)
{
    setName("Script Context");
}

ScriptContext::~ScriptContext()
{
    RBXASSERT(globalState == NULL);
}

void ScriptContext::sandboxThread(lua_State* thread)
{
    lua_newtable(thread);
    lua_newtable(thread);
    lua_pushliteral(thread, "__index");
    lua_pushvalue(thread, LUA_GLOBALSINDEX);
    lua_settable(thread, -3);
    lua_setmetatable(thread, -2);
    lua_replace(thread, LUA_GLOBALSINDEX);
}

void ScriptContext::setThreadIdentity(lua_State* thread, Security::Identities identity)
{
    lua_pushlightuserdata(thread, RBX_LUA_GLOBAL_IDENTITY);
    lua_pushinteger(thread, identity);
    lua_settable(thread, LUA_GLOBALSINDEX);

    if (DebugSettings::singleton().getioEnabled())
        _openLibInline(thread, luaopen_io, LUA_IOLIBNAME);

    lua_pushstring(thread, "settings");
    lua_pushcfunction(thread, ScriptContext::settings);
    lua_settable(thread, LUA_GLOBALSINDEX);
}

size_t ScriptContext::getThreadCount() const
{
    if (globalState)
        return RobloxExtraSpace::get(globalState)->getThreadCount();
    else
        return 0;
}

ScriptContext& ScriptContext::getContext(lua_State* thread)
{
    lua_pushlightuserdata(thread, RBX_LUA_GLOBAL_SCRIPTCONTEXT);
    lua_gettable(thread, LUA_GLOBALSINDEX);

    RBXASSERT(lua_type(thread, -1) == LUA_TLIGHTUSERDATA);
    ScriptContext* sc = static_cast<ScriptContext*>(lua_touserdata(thread, -1));
    lua_pop(thread, 1);

    return *sc;
}

static size_t pushArguments(const Reflection::ValueCollection& arguments, lua_State* thread)
{
    size_t result = 0;

    Reflection::ValueCollection::const_iterator it = arguments.begin();
    Reflection::ValueCollection::const_iterator end = arguments.end();

    for (; it != end; it++)
        result += Lua::LuaArguments::push(*it, thread);

    return result;
}

static void readResults(std::auto_ptr<Reflection::ValueCollection>& result, lua_State *thread, size_t returnCount)
{
    result.reset(new Reflection::ValueCollection(returnCount));

    if (returnCount)
    {
        for (size_t i = 0; i < returnCount; i++)
            Lua::LuaArguments::get(thread, static_cast<int>(i+1), result.get()->at(i));
    }
}

std::auto_ptr<Reflection::ValueCollection> ScriptContext::executeInNewThread(Security::Identities identity, const char* script, const char* name, const Reflection::ValueCollection& arguments)
{
    std::auto_ptr<Reflection::ValueCollection> result;
    executeInNewThread(
        identity,
        script,
        name,
        boost::bind(&pushArguments, boost::cref(arguments), _1),
        boost::bind(&readResults, boost::ref(result), _1, _2)
    );
    return result;
}

void ScriptContext::executeInNewThread(Security::Identities identity, const char* script, const char* name, PushArgumentsClosure pushArguments, ReadResultsClosure readResults)
{
    openState();

    lua_State* L;
    lua_State* newThread;

    if (identity == Security::CmdLine)
    {
        if (commandLineSandbox.empty())
        {
            {
                Lua::ThreadRef threadRef(lua_newthread(globalState));
                commandLineSandbox = threadRef;
            }

            if (commandLineSandbox.empty())
                throw std::runtime_error("Unable to create trusted sandbox thread");

            sandboxThread(commandLineSandbox.thread());
            setThreadIdentity(commandLineSandbox.thread(), Security::CmdLine);
            lua_pop(globalState, 1);
        }

        L = commandLineSandbox.thread();
        newThread = lua_newthread(L);
    }
    else
    {
        L = globalState;
        newThread = lua_newthread(L);
        sandboxThread(newThread);
        setThreadIdentity(newThread, identity);
    }

    if (!newThread)
    {
        StandardOut::singleton()->print(MESSAGE_ERROR, "ScriptContext::execute: Unable to create a new thread");
        throw std::runtime_error("Unable to create a new thread");
    }

    Lua::ScopedPopper popper(L, 1);

    std::string sName("=");

    if (name)
        sName += name;
    else
        sName += "Script";

    if (luaL_loadbuffer(newThread, script, strlen(script), sName.c_str()))
    {
        std::string err = lua_tostring(newThread, -1);
        lua_pop(newThread, 1);

        StandardOut::singleton()->print(MESSAGE_ERROR, err.c_str());
        throw std::runtime_error(err);
    }

    resume(newThread, pushArguments, readResults);
}

void ScriptContext::resume(lua_State* thread, PushArgumentsClosure pushArguments, ReadResultsClosure readResults)
{
    int nStack = lua_gettop(thread);
    size_t nArgs = pushArguments(thread);

    if (resume(thread, static_cast<int>(nArgs)) != Error)
    {
        int returnCount = lua_gettop(thread) - nStack + 1;

        try
        {
            readResults(thread, returnCount);
        }
        catch (std::exception)
        {
            lua_pop(thread, returnCount);
            throw;
        }

        lua_pop(thread, returnCount);
    }
    else
    {
        std::string err = lua_tostring(thread, -1);
        lua_settop(thread, 0);
        throw std::runtime_error(err);
    }
}

int ScriptContext::delay(lua_State* thread)
{
    // yes that's spelled correctly
    float timout = static_cast<float>(lua_tonumber(thread, 1));

    lua_State* newThread = lua_newthread(thread);
    Lua::ThreadRef functor(newThread);

    lua_pushvalue(thread, 2);
    lua_xmove(thread, functor.thread(), 1);

    getContext(thread).yieldEvent->queueWaiter(functor.thread(), timout);

    lua_pop(thread, 1);
    return 0;
}

int ScriptContext::wait(lua_State* thread)
{
    // this too
    float timout = static_cast<float>(lua_tonumber(thread, 1));
    getContext(thread).yieldEvent->queueWaiter(thread, timout);
    return lua_yield(thread, 0);
}

// TODO: functions that depend on RBX::Security

int ScriptContext::tick(lua_State* L)
{
    lua_pushnumber(L, G3D::System::getTick());
    return 1;
}

int ScriptContext::print(lua_State* L)
{
    std::string msg;
    int top = lua_gettop(L);

    lua_getglobal(L, "tostring");

    for (int i = 1; i <= top; i++)
    {
        if (i > 1)
            msg += ' ';

        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);

        const char* str = lua_tostring(L, -1);

        if (!str)
            throw std::runtime_error("'tostring' must return a string to 'print'");

        msg += str;

        if (i > 1)
            fputs("\t", stdout);

        lua_pop(L, 1);
    }

    StandardOut::singleton()->print(MESSAGE_OUTPUT, msg.c_str());

    return 0;
}

void ScriptContext::onEvent(const RunService* source, Heartbeat event)
{
    ptime t(second_clock::local_time());

    if (t >= nextPendingScripts)
    {
        startPendingScripts();
        nextPendingScripts = second_clock::local_time() + seconds(100);
    }

    if (yieldEvent)
        yieldEvent->resume(event);

    if (globalState)
        lua_gc(globalState, LUA_GCSTEP, 1);
}

void ScriptContext::startPendingScripts()
{
    if (!scriptsDisabled)
    {
        std::vector<boost::shared_ptr<Script>> copy(pendingScripts);
        pendingScripts.erase(pendingScripts.begin(), pendingScripts.end());
        std::for_each(copy.begin(), copy.end(), boost::bind(&ScriptContext::startScript, this, _1));
    }
}

void ScriptContext::onEvent(const RunService* source, RunTransition event)
{
    gc();
}

void ScriptContext::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
{
    _removeListenersInline(runService, this);

    if (oldProvider && !newProvider)
        closeState();

    if (statsItem)
    {
        statsItem->setParent(NULL);
        statsItem.reset();
    }

    Instance::onServiceProvider(oldProvider, newProvider);

    RunService* newRunService;

    if (newProvider)
    {
        Stats::StatsService* statsService = newProvider->create<Stats::StatsService>();
        if (statsService)
        {
            statsItem = Creatable<Instance>::create<Stats::Item>("Lua");
            statsItem->setParent(statsService);
            statsItem->createBoundChildItem("disabled", scriptsDisabled);
            statsItem->createChildItem<int>("threads", boost::bind(&ScriptContext::getThreadCount, this));
        }

        newRunService = newProvider->create<RunService>();
    }
    else
    {
        newRunService = NULL;
    }

    runService = shared_from(newRunService);
    _addListenerInline<RunService, RunTransition>(runService.get(), this);
    _addListenerInline<RunService, Heartbeat>(runService.get(), this);
}

void ScriptContext::onChangedScriptEnabled(const Reflection::PropertyDescriptor& __formal)
{
    if (scriptsDisabled)
        startPendingScripts();
}

void ScriptContext::addScript(Script* script)
{
    if (scripts.insert(script).second == true)
    {
        if (!script->isDisabled())
            startScript(shared_from(script));
    }
}

void ScriptContext::disassociateState(Script* script)
{
    _disassociateScriptInline(script);
}

void ScriptContext::removeScript(Script* script)
{
    if (scripts.erase(script))
    {
        _disassociateScriptInline(script);

        std::vector<boost::shared_ptr<Script>>::iterator it = std::find(pendingScripts.begin(), pendingScripts.end(), shared_from(script));
        if (it != pendingScripts.end())
            pendingScripts.erase(it);
    }
}

// TODO: 98.79% (functional match)
// incorrect placement of i initialisation
// the code here is laid out really weirdly anyway, don't know what to make of it
void ScriptContext::reportError(lua_State* thread)
{
    lua_Debug ar;

    int i = 1;

    if (lua_getstack(thread, 0, &ar))
    {
        StandardOut::singleton()->print(MESSAGE_ERROR, lua_tostring(thread, -1));

        if (DebugSettings::singleton().getStackTracingEnabled())
        {
            if (lua_getstack(thread, 1, &ar))
            {
                do
                {
                    i++;

                    lua_getinfo(thread, "nSlu", &ar);

                    StandardOut::singleton()->print(
                        MESSAGE_INFO,
                        "   stack %s, line %d: %s %s",
                        ar.source,
                        ar.currentline,
                        ar.namewhat,
                        ar.name
                    );
                }
                while (lua_getstack(thread, i, &ar));

                StandardOut::singleton()->print(MESSAGE_INFO, "   stack end");
            }
        }
    }
    else
    {
        StandardOut::singleton()->print(MESSAGE_ERROR, lua_tostring(thread, -1));
    }
}

// TODO: ScriptContext::Impersonator and ScriptContext::resume

static std::string getFullName(Instance* instance)
{
    Instance* parent = instance->getParent();

    if (parent)
    {
        if (parent->getParent())
            return getFullName(parent) + "." + instance->getName();
        else
            return instance->getName();
    }
    else 
    {
        return NULL; 
    }
}

// TODO: 99.78% (functional match)
// use of registers swapped in codegen
void ScriptContext::startScript(boost::shared_ptr<Script> script)
{
    if (scriptsDisabled)
    {
        pendingScripts.push_back(script);
        return;
    }

    boost::shared_ptr<const std::string> code = script->requestCode();

    if (!code)
    {
        pendingScripts.push_back(script);
        return;
    }

    openState();
    lua_State* thread = lua_newthread(globalState);

    if (!thread)
    {
        StandardOut::singleton()->print(MESSAGE_ERROR, "Unable to create a new thread for %s. It did not execute", script->getName().c_str());
        return;
    }

    Lua::ScopedPopper popper(globalState, 1);

    // interestingly this is just ScriptContext::sandboxThread
    lua_newtable(thread);
    lua_newtable(thread);
    lua_pushliteral(thread, "__index");
    lua_pushvalue(thread, LUA_GLOBALSINDEX);
    lua_settable(thread, -3);
    lua_setmetatable(thread, -2);
    lua_replace(thread, LUA_GLOBALSINDEX);

    setThreadIdentity(thread, Security::GameScript);

    RBXASSERT(!script->association().contains<Lua::ThreadRef::NodePtr>());
    script->association().get<Lua::ThreadRef::NodePtr>() = Lua::ThreadRef::Node::create(thread);

    RBXASSERT(lua_gettop(thread) == 0);
    Lua::ObjectBridge::push(thread, script);
    lua_setfield(thread, LUA_GLOBALSINDEX, "script");

    std::string name = std::string("=") + getFullName(script.get());

    if (luaL_loadbuffer(thread, code->c_str(), code->length(), name.c_str()))
    {
        StandardOut::singleton()->print(MESSAGE_ERROR, lua_tostring(thread, -1));
        lua_pop(thread, 1);
    }
    else
    {
        resume(thread, 0);
    }

    lua_settop(thread, 0);
}
