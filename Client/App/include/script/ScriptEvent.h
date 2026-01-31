#pragma once
#include "script/ScriptContext.h"
#include "script/ThreadRef.h"
#include "util/RunStateOwner.h"
#include "lua.h"

namespace RBX
{
    class ScriptContext;

    namespace Lua
    {
        class YieldingThreads
        {
        private:
            struct WaitingThread
            {
            public:
                boost::shared_ptr<ThreadRef> thread;
                float requestedDelay;
                float elapsedTime;
  
            public: 
                WaitingThread(lua_State*, float);
            };

        private:
            ScriptContext* context;
            std::vector<WaitingThread> waitingThreads;

        public:
            YieldingThreads(ScriptContext*);
            void queueWaiter(lua_State*, float);
            void queueWaiter(lua_State*);
            void resume(Heartbeat);
  
        private:
            void clearAllSinks();
        };
    }
}
