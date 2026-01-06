#include "AppDraw/DrawPrimitives.h"
#include "util/IndexBox.h"
#include "GLG3D/Draw.h"
#include <GL/GLU.h>

namespace RBX
{
    void DrawPrimitives::rawSphere(float radius, G3D::RenderDevice* rd)
    {
        glPushMatrix();

        GLUquadric* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluSphere(quadric, radius, 15, 15);
        gluDeleteQuadric(quadric);

        glPopMatrix();
    }

    void DrawPrimitives::rawCylinderAlongX(float radius, float axis, G3D::RenderDevice *rd)
    {
        GLUquadric* quadric;

        glPushMatrix();

        glRotatef(90.0, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, -axis*0.5);

        quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluCylinder(quadric, radius, radius, axis, 12, 1);
        gluDeleteQuadric(quadric);

        glTranslatef(0.0, 0.0, axis);

        quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluDisk(quadric, 0.0, radius, 12, 1);
        gluDeleteQuadric(quadric);
        glRotatef(180.0, 0.0, 1.0, 0.0);

        glTranslatef(0.0, 0.0, axis);

        quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluDisk(quadric, 0.0, radius, 12, 1);
        gluDeleteQuadric(quadric);

        glPopMatrix();
    }

    // TODO: 80.56%
    // this logically matches but is really difficult to get 100% matching
    // in rbxgs it allocates for and pushes f onto the stack but i can't get it to do that
    // TODO: check what cmp value gets changed to after linking
    void DrawPrimitives::rawBox(const G3D::AABox& box, G3D::RenderDevice* rd)
    {
        IndexBox indexBox(box.low(), box.high());

        int f = 0;

        G3D::Vector3 v0, v1, v2, v3;

        rd->beginPrimitive(G3D::RenderDevice::QUADS);

        do
        {
            indexBox.getFaceCorners(f, v0, v1, v2, v3);

            rd->setNormal(indexBox.getFaceNormal(f));
            rd->sendVertex(v0);
            rd->sendVertex(v1);
            rd->sendVertex(v2);
            rd->sendVertex(v3);

            f++;
        }
        while (f < 6);

        rd->endPrimitive();
    }

    void DrawPrimitives::outlineRect2d(const Rect& rect, float thick, G3D::RenderDevice* rd, const G3D::Color4& color)
    {
        float t = rect.low.x;
        float b = rect.low.y;

        float r = rect.high.x;
        float l = rect.high.y;

        rd->setColor(color);
        rd->setLineWidth(thick);
        rd->setNormal(G3D::Vector3(0.0f, 0.0f, 1.0f));

        rd->beginPrimitive(G3D::RenderDevice::LINES);
        rd->sendVertex(G3D::Vector2(t, b));

        rd->sendVertex(G3D::Vector2(r, b));
        rd->sendVertex(G3D::Vector2(r, b));

        rd->sendVertex(G3D::Vector2(r, l));
        rd->sendVertex(G3D::Vector2(r, l));

        rd->sendVertex(G3D::Vector2(t, l));
        rd->sendVertex(G3D::Vector2(t, l));

        rd->sendVertex(G3D::Vector2(t, b));
        rd->endPrimitive();
    }

    void DrawPrimitives::rect2d(const Rect& rect, G3D::RenderDevice* rd, const G3D::Color4& color)
    {
        rd->setBlendFunc(G3D::RenderDevice::BLEND_SRC_ALPHA, G3D::RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, G3D::RenderDevice::BLENDEQ_ADD);

        G3D::Draw::rect2D(
            G3D::Rect2D::xyxy(rect.low, rect.high),
            rd,
            color,
            G3D::Vector2(1.0f, 1.0f),
            G3D::Vector2(1.0f, 1.0f),
            G3D::Vector2(1.0f, 1.0f),
            G3D::Vector2(1.0f, 1.0f)
        );

        rd->setBlendFunc(G3D::RenderDevice::BLEND_ONE, G3D::RenderDevice::BLEND_ZERO, G3D::RenderDevice::BLENDEQ_ADD);
    }

    void DrawPrimitives::line2d(const G3D::Vector2& p0, const G3D::Vector2& p1, G3D::RenderDevice* rd, const G3D::Color4& color)
    {
        rd->setColor(color);
        rd->setLineWidth(1.0);
        rd->beginPrimitive(G3D::RenderDevice::LINES);
        rd->setNormal(G3D::Vector3(0.0f, 0.0f, 1.0f));
        rd->sendVertex(p0);
        rd->sendVertex(p1);
        rd->endPrimitive();
    }
}
