#pragma once
#include "RbxGraphics/Adorn.h"
#include "G3D/ReferenceCount.h"
#include "G3D/Vector2.h"
#include "G3D/Color4.h"
#include "G3D/Rect2D.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/Sphere.h"
#include "G3D/LineSegment.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/TextureManager.h"

namespace RBX
{
    class AdornG3D : public Adorn
    {
        private:
            G3D::RenderDevice* renderDevice;
            G3D::TextureManager textureManager;

        public:
            AdornG3D(const AdornG3D&);
            AdornG3D(G3D::RenderDevice*);

            G3D::RenderDevice* getRenderDevice();

            virtual TextureProxyBaseRef createTextureProxy(const std::string& filename);
            virtual G3D::Rect2D getViewport() const;

            virtual int getWidth() const;
            virtual int getHeight() const;

            virtual void setTexture(int id, TextureProxyBaseRef _texture);
            virtual G3D::Rect2D getTextureSize(TextureProxyBaseRef _texture) const;

            virtual void line2d(const G3D::Vector2& p0, const G3D::Vector2& p1, const G3D::Color4& color) const;
            virtual void outlineRect2d(const G3D::Rect2D& rect, float thick, const G3D::Color4& color) const;
            virtual void rect2d(const G3D::Rect2D& rect, const G3D::Color4& color) const;

            virtual G3D::Vector2 get2DStringBounds(const std::string& s, double size, Adorn::Spacing spacing) const;
            virtual G3D::Vector2 drawFont2D(const std::string& s, const G3D::Vector2& pos2d, double size, const G3D::Color4& color, const G3D::Color4& outline, Adorn::XAlign xalign, Adorn::YAlign yalign, Adorn::Spacing spacing) const;

            virtual void setObjectToWorldMatrix(const G3D::CoordinateFrame& c);
            virtual void setColor(const G3D::Color4& color);

            virtual void box(const G3D::AABox& box, const G3D::Color4& solidColor, const G3D::Color4& wireColor);
            virtual void sphere(const G3D::Sphere& sphere, const G3D::Color4& solidColor, const G3D::Color4& wireColor);
            virtual void explosion(const G3D::Sphere& sph, const G3D::Color4& solidColor, const G3D::Color4& wireColor);
            virtual void cylinderAlongX(float radius, float length, const G3D::Color4& solidColor, const G3D::Color4& wireColor);
            virtual void ray(const G3D::Ray& ray, const G3D::Color4& color, float scale);
            virtual void lineSegment(const G3D::LineSegment& lineSegment, const G3D::Color4& color, float scale);
            virtual void axes(const G3D::Color4& xColor, const G3D::Color4& yColor, const G3D::Color4& zColor, float scale);
            virtual void quad(const G3D::Vector3& v0, const G3D::Vector3& v1, const G3D::Vector3& v2, const G3D::Vector3& v3, const G3D::Color4& color);

            virtual ~AdornG3D();
    };
}
