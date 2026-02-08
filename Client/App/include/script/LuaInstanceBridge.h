#pragma once
#include "lua/LuaBridge.h"
#include "reflection/reflection.h"
#include "v8tree/Instance.h"
#include "lua.h"
#include "lauxlib.h"
#include "G3D/format.h"

namespace RBX
{
    namespace Lua
    {
        void newweaktable(lua_State* L, const char* mode);

        class ObjectBridge : public SharedPtrBridge<Reflection::DescribedBase>
        {
        private:
            static const luaL_Reg classLibrary[2];
  
        public:
            static int callMemberFunction(lua_State* L);
            static void registerInstanceClassLibrary(lua_State* L)
            {
                luaL_register(L, "Instance", classLibrary);
                lua_pop(L, 1);
            }
            static int newInstance(lua_State* thread);
            // TODO: 98.30% (functional match)
            static boost::shared_ptr<Instance> getInstance(lua_State* L, size_t index)
            {
                boost::shared_ptr<Reflection::DescribedBase> object = getPtr(L, index);
                Reflection::DescribedBase* object2 = object.get();

                if (object2 && !dynamic_cast<Instance*>(object2))
                    throw std::runtime_error(G3D::format("Object %s is not an Instance", object->classDescriptor().name.c_str()));

                return shared_from(static_cast<Instance*>(object2));
            }
        };

        // TODO: put this before class definition
        template <>
        int ObjectBridge::on_tostring(const boost::shared_ptr<Reflection::DescribedBase>& object, lua_State* L)
        {
            Instance* instance = dynamic_cast<Instance*>(object.get());

            if (instance)
                lua_pushstring(L, instance->getName().c_str());
            else
                lua_pushstring(L, object->classDescriptor().name.c_str());

            return 1;
        }
    }
}
