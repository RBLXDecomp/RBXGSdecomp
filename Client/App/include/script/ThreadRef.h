#pragma once
#include "lua.h"
#include "lua/LuaBridge.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"

namespace RBX
{
    namespace Lua
    {
        class FunctionRef;
        FunctionRef lua_tofunction(lua_State* L, int index);
        void lua_pushfunction(lua_State* L, const FunctionRef& function);
        
        class ThreadRef 
        {
        public:
            class Node;
            typedef boost::shared_ptr<Node> NodePtr;
            typedef Bridge<NodePtr, true> NodeBridge;

        public:
            class Node 
            {
                friend class ThreadRef;

            private:
                ThreadRef* first;
                const boost::shared_ptr<boost::mutex> sync;
            
            private: 
                Node()
                    : first(NULL),
                      sync(syncSingleton)
                {
                }

            public: 
                ~Node();
                void eraseAllRefs();
            
            public:
                static NodePtr create(lua_State* thread);
                static NodePtr get(lua_State* thread);
            };

        private:
            const boost::shared_ptr<boost::mutex> sync;
            Node* node;
            ThreadRef* previous;
            ThreadRef* next;
            lua_State* L;
            int threadId;
            
        private:
            static boost::shared_ptr<boost::mutex> syncSingleton;
        
        private:
            void addRef()
            {
                if (L)
                {
                    lua_pushthread(L);
                    threadId = luaL_ref(L, LUA_REGISTRYINDEX);
                }
            }
            void addToNode()
            {
                if (node)
                {
                    ThreadRef* first = node->first;
                    if (first)
                    {
                        next = first;
                        first->previous = this;
                    }
                    else
                    {
                        next = NULL;
                    }

                    previous = NULL;
                    node->first = this;
                }
            }
            void removeFromNode()
            {
                if (node)
                {
                    if (next)
                        next->previous = previous;

                    if (previous)
                        previous->next = next;

                    if (node->first == this)
                        node->first = next;

                    next = NULL;
                    previous = NULL;
                    node = NULL;
                }
            }

        protected: 
            virtual void removeRef();

        public:
            ThreadRef(const ThreadRef& other);
            ThreadRef(lua_State* thread);
            ThreadRef()
                : sync(syncSingleton),
                  node(NULL),
                  previous(NULL),
                  next(NULL),
                  L(NULL),
                  threadId(0)
            {
            }
            ThreadRef& operator=(const ThreadRef& other);
            ~ThreadRef();

            bool operator==(const ThreadRef&) const;
            bool operator!=(const ThreadRef&) const;

            void reset();
            bool empty() const
            {
                return !L;
            }
            lua_State* thread() const
            {
                return L;
            }
        };

        template<>
        int ThreadRef::NodeBridge::on_tostring(const ThreadRef::NodePtr& object, lua_State* L)
        {
            lua_pushstring(L, "ThreadRef");
            return 1;
        }

        class FunctionRef : public ThreadRef
        {
        // TODO: how is lua_pushfunction supposed to access this when private?
        // private:
        public:
            int functionId;

        public:
            FunctionRef(const FunctionRef& other);
            FunctionRef(lua_State* thread, int functionIndex);
            FunctionRef();
            ~FunctionRef();

        public:
            FunctionRef& operator=(const FunctionRef&);
            bool operator==(const FunctionRef&) const;
            bool operator!=(const FunctionRef&) const;

        protected:
            virtual void removeRef();
        };
    }
}
