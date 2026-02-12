#pragma once
#include "script/Script.h"
#include "script/ScriptEvent.h"
#include "script/LuaMemory.h"
#include "script/LuaArguments.h"
#include "script/ThreadRef.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "v8datamodel/Stats.h"
#include "util/Events.h"
#include "util/RunStateOwner.h"
#include "boost/shared_ptr.hpp"
#include "boost/date_time.hpp"
#include "lua.h"

// TODO: where do these numbers actually come from? i don't think
// they were actually defined like this
#define RBX_LUA_GLOBAL_SCRIPTCONTEXT (void*)0x43 // 'C'
#define RBX_LUA_GLOBAL_THREADREFNODE (void*)0x4E // 'N'
#define RBX_LUA_GLOBAL_IDENTITY (void*)0x54 // 'T'

namespace RBX
{
    class Script;

    namespace Lua
    {
        class YieldingThreads;
    }

    // TODO: remove when appropriate
    namespace Security
    {
        enum Identities
        {
            Anonymous,
            LocalGUI,
            GameScript,
            CmdLine,
            TrustedCOM,
            TrustedWebService,
            Replicator
        };
    }

    extern const char *sScriptContext;
    class ScriptContext : public DescribedCreatable<ScriptContext, Instance, &sScriptContext>,
                          public Service,
                          public Listener<RunService, RunTransition>,
                          public Listener<RunService, Heartbeat>
    {
    public:
        typedef boost::function1<size_t, lua_State*> PushArgumentsClosure;
        typedef boost::function2<void, lua_State*, size_t> ReadResultsClosure;

    public:
        enum Result
        {
            Success,
            Yield,
            Error
        };

        // TODO: come back to this when luasoft gets his stuff done
    public:
        class ScriptImpersonator// : public Security::Impersonator
        {
        public:
            ScriptImpersonator(lua_State*);
            ~ScriptImpersonator();
        };

    private:
        lua_State* globalState;
        Lua::ThreadRef commandLineSandbox;
        std::set<Script*> scripts;
        boost::posix_time::ptime nextPendingScripts;
        std::vector<boost::shared_ptr<Script>> pendingScripts;
        boost::shared_ptr<RunService> runService;
        boost::scoped_ptr<Lua::YieldingThreads> yieldEvent;
        bool scriptsDisabled;
        boost::shared_ptr<Stats::Item> statsItem;
    
    public:
        boost::scoped_ptr<LuaAllocator> allocator;
        static Reflection::BoundProp<bool, true> propScriptsDisabled;
    
    public:
        ScriptContext();
        virtual ~ScriptContext();

        std::auto_ptr<Reflection::ValueCollection> call(const Lua::FunctionRef&, const Reflection::ValueCollection&);
        void call(const Lua::FunctionRef&);
        void call(const Lua::FunctionRef&, PushArgumentsClosure, ReadResultsClosure);
        std::auto_ptr<Reflection::ValueCollection> executeInNewThread(Security::Identities, const char*, const char*, const Reflection::ValueCollection&);
        void executeInNewThread(Security::Identities, const char*, const char*);
        void executeInNewThread(Security::Identities, const char*, const char*, PushArgumentsClosure pushArguments, ReadResultsClosure readResults);
        Result resume(lua_State* thread, int narg);
        void resume(lua_State* thread, PushArgumentsClosure pushArguments, ReadResultsClosure readResults);
        void gc()
        {
            if (globalState)
                lua_gc(globalState, LUA_GCCOLLECT, 0);
        }
        size_t getThreadCount() const;
        bool hasState() const;
        lua_State* state();
        virtual XmlElement* write();
    
    protected:
        virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
        virtual void onEvent(const RunService* source, Heartbeat event);
        virtual void onEvent(const RunService* source, RunTransition event);
      
    private:
        void onChangedScriptEnabled(const Reflection::PropertyDescriptor& __formal);
      
    public:
        void closeState();
        void addScript(Script* script);
        void removeScript(Script* script);
      
    private:
        void disassociateState(Script* script);
        void openState();
        void startScript(boost::shared_ptr<Script> script);
        void startPendingScripts();
    
    public: 
        static ScriptContext& getContext(lua_State* thread);

    private:
        static void sandboxThread(lua_State* thread);
        static void setThreadIdentity(lua_State* thread, Security::Identities identity);
        static Security::Identities getThreadIdentity(lua_State* thread);
        static int print(lua_State* L);
        static int tick(lua_State* L);
        static int wait(lua_State* thread);
        static int delay(lua_State* thread);
        static int trustedThread(lua_State*);
        static int loadfile(lua_State*);
        static int dofile(lua_State*);
        static int settings(lua_State*);
        static int stats(lua_State*);
        static int statsitemvalue(lua_State*);
        static int onError(lua_State*);
        static void reportError(lua_State* thread);
    };
}
