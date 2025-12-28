#pragma once
#include "RbxGraphics/TextureProxyBase.h"
#include <G3D/Color3uint8.h>
#include <GLG3D/RenderDevice.h>
#include <GLG3D/Texture.h>
#include <GLG3D/TextureManager.h>

namespace RBX
{
	namespace Render
	{
		class TextureProxy : public TextureProxyBase
		{
		protected:
			G3D::ReferenceCountedPointer<G3D::Texture> texture;
			bool loaded;
			G3D::TextureManager* textureManager;
			bool tiled;
			G3D::ReferenceCountedPointer<TextureProxy> root;
			G3D::ReferenceCountedPointer<TextureProxy> shadeTexture;
			G3D::Color3 shadeColor;
		public:
			//TextureProxy(const TextureProxy&);
			TextureProxy(G3D::TextureManager& textureManager, const std::string& filename, bool tiled)
				: textureManager(&textureManager),
				loaded(false),
				tiled(tiled)
			{
				this->filename = filename;
			}
		protected:
			TextureProxy(G3D::ReferenceCountedPointer<TextureProxy> root)
				: root(root),
				loaded(false),
				textureManager(NULL)
			{
				this->filename = "";
			}
		public:
			G3D::ReferenceCountedPointer<TextureProxy> shade(const G3D::Color3& baseColor, const G3D::ReferenceCountedPointer<TextureProxy>& baseTex);
			G3D::ReferenceCountedPointer<TextureProxy> alphaOnlyVersion();
			virtual G3D::ReferenceCountedPointer<G3D::Texture> getIfResolved() const;
			virtual G3D::ReferenceCountedPointer<G3D::Texture> resolve(G3D::RenderDevice* rd);
			virtual G3D::Vector2 getSize();
			virtual ~TextureProxy();
			//TextureProxy& operator=(const TextureProxy&);

			static void shadePixel(const G3D::Color3uint8&, G3D::Color3uint8&);
		};
	}
}
