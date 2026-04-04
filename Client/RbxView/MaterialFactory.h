#pragma once
#include "RenderLib/Material.h"
#include "v8datamodel/BrickColor.h"
#include <GLG3D/TextureManager.h>

namespace RBX
{
	namespace View
	{
		class MaterialFactory
		{
			struct Attributes
			{
				const BrickColor color;
				const float transparency;
				const float reflectance;

				bool operator<(const Attributes&) const;
			};

		private:
			G3D::ReferenceCountedPointer<Render::TextureProxy> megaTexture;
			G3D::ReferenceCountedPointer<Render::TextureProxy> surfacesTexture;
			G3D::ReferenceCountedPointer<Render::TextureProxy> fileMeshTexture;
			G3D::ReferenceCountedPointer<Render::Material> fileMeshMaterial;
			std::map<Attributes, G3D::WeakReferenceCountedPointer<Render::Material>> database;
			std::map<Attributes, G3D::WeakReferenceCountedPointer<Render::Material>> megaDatabase;

		public:
			G3D::ReferenceCountedPointer<Render::Material> getMegaMaterial(Attributes);
			G3D::ReferenceCountedPointer<Render::Material> getFileMeshMaterial();
			G3D::ReferenceCountedPointer<Render::Material> getMaterial(Attributes attributes);

			MaterialFactory(G3D::TextureManager* textureManager);
		private:
			G3D::ReferenceCountedPointer<Render::TextureProxy> getSurfacesTexture(G3D::Color3 tint);
		};
		
		class MegaTextureProxy : public Render::TextureProxy
		{
			friend class MaterialFactory;

		private:
			float _stripWidth;
		public:
			static const size_t numStrips = 20;

		private:
			MegaTextureProxy(G3D::TextureManager& textureManager, const std::string& file)
				: Render::TextureProxy(textureManager, file, false),
				  _stripWidth(0.0f)
			{
			}
			virtual G3D::ReferenceCountedPointer<G3D::Texture> resolve(G3D::RenderDevice*);
		};
	}
}
