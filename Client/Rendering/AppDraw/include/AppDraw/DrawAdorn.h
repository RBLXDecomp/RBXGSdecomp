#pragma once
#include "AppDraw/HandleType.h"
#include "AppDraw/Part.h"
#include "RbxGraphics/Adorn.h"
#include "G3D/GCamera.h"

namespace RBX
{
    class DrawAdorn
    {
    private:
        static void surfaceBorder(Adorn* adorn, const G3D::Vector3& halfRealSize, float highlight, int surfaceId, const G3D::Color4& color);

    public:
        static const G3D::Color3& resizeColor();
        static void cylinder(Adorn* adorn, const G3D::CoordinateFrame& worldC, int axis, float length, float radius, const G3D::Color3& color);
        static void partSurface(const Part& part, int surfaceId, Adorn* adorn, const G3D::Color4& color);
        static void handles3d(const G3D::Vector3&, const G3D::CoordinateFrame&, Adorn*, HandleType, const G3D::Vector3&);
        static void handles2d(const G3D::Vector3&, const G3D::CoordinateFrame&, const G3D::GCamera&, Adorn*, const G3D::Color3&);
    };
}
