#pragma once
#include "G3D/AABox.h"
#include "G3D/Vector2.h"
#include "G3D/Color4.h"
#include "GLG3D/RenderDevice.h"
#include "util/Rect.h"

namespace RBX
{
    class DrawPrimitives
    {
    public:
        static void rawBox(const G3D::AABox& box, G3D::RenderDevice* rd);
        static void rawSphere(float radius, G3D::RenderDevice* rd);
        static void rawCylinderAlongX(float radius, float axis, G3D::RenderDevice* rd);
        static void rect2d(const Rect& rect, G3D::RenderDevice* rd, const G3D::Color4& color);
        static void line2d(const G3D::Vector2& p0, const G3D::Vector2& p1, G3D::RenderDevice* rd, const G3D::Color4& color);
        static void outlineRect2d(const Rect& rect, float thick, G3D::RenderDevice* rd, const G3D::Color4& color);
    };
}
