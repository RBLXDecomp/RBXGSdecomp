#pragma once
#include "RenderLib/TextureProxy.h"
#include "RbxGraphics/Adorn.h"
#include "util/ContentProvider.h"

namespace RBX
{
    class Textures 
    {
    public: 
        static TextureProxyBaseRef getTextureProxy(Adorn* adorn, const ContentId& id);
    };
}
