#include "AppDraw/Textures.h"

namespace RBX
{
    TextureProxyBaseRef Textures::getTextureProxy(Adorn* adorn, const ContentId& id)
    {
        try
        {
            std::string fileName = ContentProvider::singleton().getFile(id);
            return adorn->createTextureProxy(fileName);
        }
        catch (std::exception)
        {
            return TextureProxyBaseRef();
        }
    }
}
