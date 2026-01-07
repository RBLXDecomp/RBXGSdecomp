#pragma once
#include "G3D/ReferenceCount.h"
#include "GLG3D/GFont.h"

namespace RBX
{
    class Fonts
    {
    private:
        static G3D::GFontRef fontRef;

    public:
        static G3D::GFontRef getFont(int);
        static G3D::GFontRef getFont();
    };
}
