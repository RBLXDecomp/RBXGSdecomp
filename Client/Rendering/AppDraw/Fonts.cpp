#include "AppDraw/Fonts.h"
#include "util/ContentProvider.h"

namespace RBX
{
    static G3D::GFontRef fontRef;

    G3D::GFontRef Fonts::getFont()
    {
        if (fontRef.isNull())
        {
            try
            {
                std::string filePath = "Fonts\\comics.fnt";
                fontRef = G3D::GFont::fromFile(ContentProvider::singleton().getAssetFile(filePath));
            }
            catch (std::exception)
            {
            }
        }

        return fontRef;
    }
}
