#pragma once
#include <string>
#include <G3D/Vector2.h>
#include <G3D/Rect2D.h>
#include <G3D/Color4.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/AABox.h>
#include <G3D/Sphere.h>
#include <G3D/LineSegment.h>
#include "RbxGraphics/TextureProxyBase.h"

namespace RBX
{
	class Adorn
	{
	public:
		enum Spacing
		{
			PROPORTIONAL_SPACING,
			FIXED_SPACING
		};
		enum XAlign
		{
			XALIGN_RIGHT,
			XALIGN_LEFT,
			XALIGN_CENTER
		};
		enum YAlign
		{
			YALIGN_TOP,
			YALIGN_BASELINE,
			YALIGN_CENTER,
			YALIGN_BOTTOM
		};

	public:
		Adorn()
		{
		}
		virtual ~Adorn()
		{
		}
	public:
		virtual G3D::ReferenceCountedPointer<TextureProxyBase> createTextureProxy(const std::string& filename) = 0;
		virtual void prepareRenderPass() = 0;
		virtual void finishRenderPass() = 0;
		virtual G3D::Rect2D getViewport() const = 0;
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual void setTexture(int id, G3D::ReferenceCountedPointer<TextureProxyBase> _texture) = 0;
		virtual G3D::Rect2D getTextureSize(G3D::ReferenceCountedPointer<TextureProxyBase> _texture) const = 0;
		virtual void line2d(const G3D::Vector2& p0, const G3D::Vector2& p1, const G3D::Color4& color) const = 0;
		virtual void outlineRect2d(const G3D::Rect2D& rect, float thick, const G3D::Color4& color) const = 0;
		virtual void rect2d(const G3D::Rect2D& rect, const G3D::Color4& color) const = 0;
		virtual G3D::Vector2 get2DStringBounds(const std::string& s, double size, Spacing spacing) const = 0;
		virtual G3D::Vector2 drawFont2D(const std::string& s, const G3D::Vector2& pos2d, double size, const G3D::Color4& color, const G3D::Color4& outline, XAlign xalign, YAlign yalign, Spacing spacing) const = 0;
		virtual void setObjectToWorldMatrix(const G3D::CoordinateFrame& c) = 0;
		virtual void setColor(const G3D::Color4& color) = 0;
		virtual void box(const G3D::AABox& box, const G3D::Color4& solidColor, const G3D::Color4& wireColor) = 0;
		virtual void sphere(const G3D::Sphere& sphere, const G3D::Color4& solidColor, const G3D::Color4& wireColor) = 0;
		virtual void explosion(const G3D::Sphere& sph, const G3D::Color4& solidColor, const G3D::Color4& wireColor) = 0;
		virtual void cylinderAlongX(float radius, float length, const G3D::Color4& solidColor, const G3D::Color4& wireColor) = 0;
		virtual void ray(const G3D::Ray& ray, const G3D::Color4& color, float scale) = 0;
		virtual void lineSegment(const G3D::LineSegment& lineSegment, const G3D::Color4& color, float scale) = 0;
		virtual void axes(const G3D::Color4& xColor, const G3D::Color4& yColor, const G3D::Color4& zColor, float scale) = 0;
		virtual void quad(const G3D::Vector3& v0, const G3D::Vector3& v1, const G3D::Vector3& v2, const G3D::Vector3& v3, const G3D::Color4& color) = 0;
	};
}
