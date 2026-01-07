#pragma once
#include "AppDraw/Part.h"
#include "RbxGraphics/Adorn.h"
#include "G3D/GCamera.h"

namespace RBX
{
    // TODO: is this supposed to be here?
    enum HandleType
    {
        HANDLE_RESIZE   = 0,
        HANDLE_MOVE     = 1,
        HANDLE_ROTATE   = 2,
        HANDLE_VELOCITY = 3
    };

    class DrawAdorn
    {
    private:
        static void surfaceBorder(Adorn*, const G3D::Vector3&, float, int, const G3D::Color4&);

    public:
        static const G3D::Color3& resizeColor();
        static void cylinder(Adorn*, const G3D::CoordinateFrame&, int, float, float, const G3D::Color3&);
        static void partSurface(const Part&, int, Adorn*, const G3D::Color4&);
        static void handles3d(const G3D::Vector3&, const G3D::CoordinateFrame&, Adorn*, HandleType, const G3D::Vector3&);
        static void handles2d(const G3D::Vector3&, const G3D::CoordinateFrame&, const G3D::GCamera&, Adorn*, const G3D::Color3&);
    };
}
