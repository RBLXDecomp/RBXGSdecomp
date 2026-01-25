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
            class Node 
            {
            private:
                ThreadRef* first;
                const boost::shared_ptr<boost::mutex> sync;
            
            /* public:
                Node(const Node&); */

            private: 
                Node();

            public: 
                ~Node();
                void eraseAllRefs();
            
            public:
                static boost::shared_ptr<Node> create(lua_State*);
                static boost::shared_ptr<Node> get(lua_State*);
            };

        private:
            const boost::shared_ptr<boost::mutex> sync;
            Node* node;
            ThreadRef* previous;
            ThreadRef* next;
            lua_State* L;
            int threadId;
            static boost::shared_ptr<boost::mutex> syncSingleton;
        
        private:
            void addRef();
            void addToNode();
            void removeFromNode();

        protected: 
            virtual void removeRef();

        public:
            // ThreadRef(const ThreadRef&);
            ThreadRef(lua_State*);
            ThreadRef();
            // ThreadRef& operator=(const ThreadRef&);
            ~ThreadRef();

            bool operator==(const ThreadRef&) const;
            bool operator!=(const ThreadRef&) const;

            void reset();
            bool empty() const;
            lua_State* thread() const;
        };
    }
}
