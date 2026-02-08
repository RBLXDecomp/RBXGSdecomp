#include "script/ThreadRef.h"
#include "script/ScriptContext.h"
#include "lua.h"
#include "lauxlib.h"

namespace RBX
{
    namespace Lua
    {
        ThreadRef::ThreadRef(lua_State* thread)
            : sync(syncSingleton),
              node(NULL),
              previous(NULL),
              next(NULL),
              L(thread),
              threadId(0)
        {
            boost::mutex::scoped_lock lock(*sync);

            node = Node::get(thread).get();
            addToNode();
            addRef();
        }

        ThreadRef::ThreadRef(const ThreadRef& other)
            : sync(syncSingleton),
              node(other.node),
              L(other.L)
        {
            boost::mutex::scoped_lock lock(*sync);

            addRef();
            addToNode();
        }

        ThreadRef::~ThreadRef()
        {
            reset();
        }

        void ThreadRef::reset()
        {
            boost::mutex::scoped_lock lock(*sync);

            removeFromNode();
            removeRef();
        }

        ThreadRef& ThreadRef::operator=(const ThreadRef& other)
        {
            if (L != other.L)
            {
                boost::mutex::scoped_lock lock(*sync);

                removeRef();
                L = other.L;

                if (L)
                {
                    lua_pushthread(L);
                    threadId = luaL_ref(L, LUA_REGISTRYINDEX);
                }
            }

            if (node != other.node)
            {
                boost::mutex::scoped_lock lock(*sync);

                removeFromNode();

                node = other.node;
                addToNode();
            }

            return *this;
        }

        void ThreadRef::removeRef()
        {
            if (L)
            {
                lua_unref(L, threadId);
                threadId = 0;
                L = NULL;
            }
        }

        void ThreadRef::Node::eraseAllRefs()
        {
            boost::mutex::scoped_lock lock(*sync);

            for (ThreadRef* ref = first; ref; ref = ref->next)
            {
                ref->removeRef();
                ref->node = NULL;
            }

            first = NULL;
        }

        ThreadRef::Node::~Node()
        {
            eraseAllRefs();
        }

        ThreadRef::NodePtr ThreadRef::Node::create(lua_State* thread)
        {
            lua_pushlightuserdata(thread, RBX_LUA_GLOBAL_THREADREFNODE);
            ThreadRef::NodePtr* newPtr = NodeBridge::pushNewObject(thread);
            lua_settable(thread, LUA_GLOBALSINDEX);

            *newPtr = ThreadRef::NodePtr(new Node());
            return *newPtr;
        }

        ThreadRef::NodePtr ThreadRef::Node::get(lua_State* thread)
        {
            lua_pushlightuserdata(thread, RBX_LUA_GLOBAL_THREADREFNODE);
            lua_gettable(thread, LUA_GLOBALSINDEX);
            ThreadRef::NodePtr& node = NodeBridge::getObject(thread, lua_gettop(thread));
            lua_pop(thread, 1);
            return node;
        }

        template<>
        int ThreadRef::NodeBridge::on_index(const ThreadRef::NodePtr& object, const char* name, lua_State* L)
        {
            throw std::runtime_error(G3D::format("%s is not a valid member", name));
        }

        FunctionRef::FunctionRef(lua_State* thread, int functionIndex)
            : ThreadRef(thread)
        {
            lua_pushvalue(thread, functionIndex);
            functionId = luaL_ref(thread, LUA_REGISTRYINDEX);
        }

        FunctionRef lua_tofunction(lua_State* L, int index)
        {
            return FunctionRef(L, index);
        }

        void lua_pushfunction(lua_State* L, const FunctionRef& function)
        {
            lua_getref(L, function.functionId);
        }

        FunctionRef::~FunctionRef()
        {
            if (functionId && !empty())
                lua_unref(thread(), functionId);  
        }

        void FunctionRef::removeRef()
        {
            if (functionId && !empty())
            {
                lua_unref(thread(), functionId);  
                functionId = 0;
            }

            ThreadRef::removeRef();
        }

        FunctionRef::FunctionRef(const FunctionRef& other)
            : ThreadRef(other)
        {
            if (empty())
            {
                functionId = 0;
            }
            else
            {
                lua_getref(thread(), other.functionId);
                functionId = luaL_ref(thread(), LUA_REGISTRYINDEX);
            }
        }
    }

    namespace Reflection
    {
		template<>
		const Type& Type::singleton<Lua::FunctionRef>()
		{
			static Type type("Function", typeid(Lua::FunctionRef));
			return type;
		}
    }
}