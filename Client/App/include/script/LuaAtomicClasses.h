#pragma once
#include "v8datamodel/BrickColor.h"
#include "lua/LuaBridge.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/Vector3.h"
#include "G3D/Color3.h"
#include "lauxlib.h"

namespace RBX
{
    namespace Lua
    {
        class CoordinateFrameBridge : public Bridge<G3D::CoordinateFrame, true>
        {
        private:
            static const luaL_Reg classLibrary[0];
  
        public:
            static void registerClassLibrary(lua_State* L)
            {
                luaL_register(L, className, classLibrary);
                lua_pop(L, 1);
            }
            static void pushCoordinateFrame(lua_State*, G3D::CoordinateFrame);
        
        private:
            static int newCoordinateFrame(lua_State*);
            static int fromEulerAnglesXYZ(lua_State*);
            static int fromAxisAngle(lua_State*);
            static int on_add(lua_State*);
            static int on_sub(lua_State*);
            static int on_mul(lua_State*);
            static int on_inverse(lua_State*);
            static int on_toWorldSpace(lua_State*);
            static int on_toObjectSpace(lua_State*);
            static int on_pointToWorldSpace(lua_State*);
            static int on_pointToObjectSpace(lua_State*);
            static int on_vectorToWorldSpace(lua_State*);
            static int on_vectorToObjectSpace(lua_State*);
            static int on_toEulerAnglesXYZ(lua_State*);
            static int on_components(lua_State*);
        };

        class Vector3Bridge : public Bridge<G3D::Vector3, true>
        {
        private:
            static const luaL_Reg classLibrary[0];

        public:
            static void registerClassLibrary(lua_State* L)
            {
                luaL_register(L, className, classLibrary);
                lua_pop(L, 1);
            }
            static void pushVector3(lua_State*, G3D::Vector3);
  
        private:
            static int newVector3(lua_State*);
            static int on_add(lua_State*);
            static int on_sub(lua_State*);
            static int on_mul(lua_State*);
            static int on_div(lua_State*);
            static int on_unm(lua_State*);
        };


        class Color3Bridge : public Bridge<G3D::Color3, true>
        {
        private:
            static const luaL_Reg classLibrary[0];
        
        public:
            static void registerClassLibrary(lua_State* L)
            {
                luaL_register(L, className, classLibrary);
                lua_pop(L, 1);
            }
            static void pushColor3(lua_State*, G3D::Color3);

        private:
            static int newColor3(lua_State*);
        };

        class BrickColorBridge : public Bridge<BrickColor, true>
        {
        private:
            static const luaL_Reg classLibrary[0];
        
        public:
            static void registerClassLibrary(lua_State* L)
            {
                luaL_register(L, className, classLibrary);
                lua_pop(L, 1);
            }
            
        private:
            static int newBrickColor(lua_State*);
            static int randomBrickColor(lua_State*);
        };
    }
}
