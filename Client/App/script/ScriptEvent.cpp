#include "script/ScriptEvent.h"

namespace RBX
{
    namespace Lua
    {
        YieldingThreads::YieldingThreads(ScriptContext* context)
            : context(context)
        {
        }

        void YieldingThreads::queueWaiter(lua_State* L)
        {
            queueWaiter(L, 0.0f);
        }

        void YieldingThreads::queueWaiter(lua_State* L, float delay)
        {
            RobloxExtraSpace::get(L)->yieldCaptured = true;
            waitingThreads.push_back(WaitingThread(L, delay));
        }

        void YieldingThreads::resume(Heartbeat heartbeat)
        {
            if (waitingThreads.empty())
                return;

            std::vector<WaitingThread> copy = waitingThreads;
            waitingThreads.erase(waitingThreads.begin(), waitingThreads.end());

            std::vector<WaitingThread>::iterator it = copy.begin();
            for (; it != copy.end(); it++)
            {
                if (it->thread->empty())
                    continue;

                // TODO: LHS and RHS swapped in codegen
                it->elapsedTime += heartbeat.step;

                if (it->elapsedTime < it->requestedDelay)
                {
                    waitingThreads.push_back(*it);
                    continue;
                }

                lua_State* L = it->thread->thread();
                const int oldTop = lua_gettop(L);
                lua_pushnumber(L, it->elapsedTime);
                context->resume(L, 1);
                lua_settop(L, oldTop);
            }
        }
    }
}
