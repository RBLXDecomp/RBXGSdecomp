#include "RenderLib/TextureProxy.h"

namespace RBX
{
	namespace Render
	{
		G3D::ReferenceCountedPointer<G3D::Texture> TextureProxy::getIfResolved() const
		{
			return texture;
		}

		G3D::ReferenceCountedPointer<TextureProxy> TextureProxy::alphaOnlyVersion()
		{
			return new TextureProxy(this);
		}

		G3D::Vector2 TextureProxy::getSize()
		{
			G3D::ReferenceCountedPointer<G3D::Texture> t = getIfResolved();
			G3D::Vector2 out;
			if (t.notNull())
			{
				out.x = t->texelWidth();
				out.y = t->texelHeight();
			}
			else
			{
				out.y = 0.0f;
				out.x = 0.0f;
			}
			return out;
		}

		G3D::ReferenceCountedPointer<TextureProxy> TextureProxy::shade(const G3D::Color3& baseColor, const G3D::ReferenceCountedPointer<TextureProxy>& baseTex)
		{
			TextureProxy* newTex = new TextureProxy(baseTex);
			newTex->textureManager = textureManager;
			newTex->root = baseTex;
			newTex->shadeColor = baseColor;
			newTex->shadeTexture = this;
			return newTex;
		}
	}
}
