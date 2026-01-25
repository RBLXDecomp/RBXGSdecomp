#pragma once
#include "script/Script.h"
#include "script/LuaMemory.h"
#include "script/ThreadRef.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "util/Events.h"
#include "util/RunStateOwner.h"
#include "boost/shared_ptr.hpp"
#include "boost/date_time.hpp"
#include "lua.h"

namespace RBX
{
    class Script; // this is intentional, don't remove this

    // TODO: remove forward declarations when implemented
    namespace Lua
    {
        class YieldingThreads{};
        class FunctionRef{};
    }

    namespace Security
    {
        enum Identities{};
    }

    namespace Stats
    {
        class Item{};
    }

    extern const char *sScriptContext;
    class ScriptContext : public DescribedCreatable<ScriptContext, Instance, &sScriptContext>,
                          public Service,
                          public Listener<RunService, RunTransition>,
                          public Listener<RunService, Heartbeat>
    {
    public:
        enum Result
        {
            Success,
            Yield,
            Error
        };

        // TODO: come back to this when luasoft gets his stuff done
        class ScriptImpersonator// : public Security::Impersonator
        {
        public:
            ScriptImpersonator(lua_State*);
            ~ScriptImpersonator();
        };

    private:
        lua_State* globalState;
        Lua::ThreadRef commandLineSandbox;
        std::set<Script*, std::less<Script*>, std::allocator<Script*>> scripts;
        boost::posix_time::ptime nextPendingScripts;
        std::vector<boost::shared_ptr<Script>, std::allocator<boost::shared_ptr<Script>>> pendingScripts;
        boost::shared_ptr<RunService> runService;
        boost::scoped_ptr<Lua::YieldingThreads> yieldEvent;
        bool scriptsDisabled;
        boost::shared_ptr<Stats::Item> statsItem;
    
    public:
        boost::scoped_ptr<LuaAllocator> allocator;
        static Reflection::BoundProp<bool, true> propScriptsDisabled;
    
    public:
        ScriptContext(const ScriptContext&);
        ScriptContext();
        virtual ~ScriptContext();

        // TODO: typedef these long repeated types somewhere?
        std::auto_ptr<std::vector<Reflection::Value, std::allocator<Reflection::Value>>> call(const Lua::FunctionRef&, const std::vector<Reflection::Value, std::allocator<Reflection::Value>>&);
        void call(const Lua::FunctionRef&);
        void call(const Lua::FunctionRef&, boost::function1<size_t, lua_State*, std::allocator<boost::function_base>>, boost::function2<void, lua_State*, size_t, std::allocator<boost::function_base>>);
        std::auto_ptr<std::vector<Reflection::Value, std::allocator<Reflection::Value>>> executeInNewThread(Security::Identities, const char*, const char*, const std::vector<Reflection::Value,std::allocator<Reflection::Value>>&);
        void executeInNewThread(Security::Identities, const char*, const char*);
        void executeInNewThread(Security::Identities, const char*, const char*, boost::function1<size_t, lua_State*, std::allocator<boost::function_base>>, boost::function2<void, lua_State*, size_t, std::allocator<boost::function_base>>);
        ScriptContext::Result resume(lua_State*, int);
        void resume(lua_State*, boost::function1<size_t, lua_State*, std::allocator<boost::function_base>>, boost::function2<void, lua_State*, size_t, std::allocator<boost::function_base>>);
        void gc();
        size_t getThreadCount() const;
        bool hasState() const;
        lua_State* state();
        virtual XmlElement* write();
    
    protected:
        virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
        virtual void onEvent(const RunService*, Heartbeat);
        virtual void onEvent(const RunService*, RunTransition);
      
    private:
        void onChangedScriptEnabled(const Reflection::PropertyDescriptor&);
      
    public:
        void closeState();
        void addScript(Script*);
        void removeScript(Script*);
      
    private:
        void disassociateState(Script*);
        void openState();
        void startScript(boost::shared_ptr<Script>);
        void startPendingScripts();
    
    public: 
        static ScriptContext& getContext(lua_State*);

    private:
        static void sandboxThread(lua_State*);
        static void setThreadIdentity(lua_State*, Security::Identities);
        static Security::Identities getThreadIdentity(lua_State*);
        static int print(lua_State*);
        static int tick(lua_State*);
        static int wait(lua_State*);
        static int delay(lua_State*);
        static int trustedThread(lua_State*);
        static int loadfile(lua_State*);
        static int dofile(lua_State*);
        static int settings(lua_State*);
        static int stats(lua_State*);
        static int statsitemvalue(lua_State*);
        static int onError(lua_State*);
        static void reportError(lua_State*);
    };
}
