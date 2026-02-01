#pragma once
#include "lua.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"

namespace RBX
{
    namespace Lua
    {
        class ThreadRef 
        {
        public:
            class Node;
            typedef boost::shared_ptr<Node> NodePtr;

        public:
            class Node 
            {
            private:
                ThreadRef* first;
                const boost::shared_ptr<boost::mutex> sync;
            
            private: 
                Node();

            public: 
                ~Node();
                void eraseAllRefs();
            
            public:
                static NodePtr create(lua_State*);
                static NodePtr get(lua_State*);
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
            void addRef();
            void addToNode();
            void removeFromNode();

        protected: 
            virtual void removeRef();

        public:
            ThreadRef(const ThreadRef&);
            ThreadRef(lua_State*);
            ThreadRef()
                : sync(syncSingleton),
                  node(NULL),
                  previous(NULL),
                  next(NULL),
                  L(NULL),
                  threadId(NULL)
            {
            }
            ThreadRef& operator=(const ThreadRef&);
            ~ThreadRef();

            bool operator==(const ThreadRef&) const;
            bool operator!=(const ThreadRef&) const;

            void reset();
            bool empty() const
            {
                // TODO: is this correct?
                return !L;
            }
            lua_State* thread() const
            {
                return L;
            }
        };

        class FunctionRef : public ThreadRef
        {
        private:
            size_t functionId;

        public:
            FunctionRef(const FunctionRef&);
            FunctionRef(lua_State*, size_t);
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
