#include "MaterialFactory.h"
#include "util/ContentProvider.h"

static const float specular = 0.9f;
static const float shiny = 50.0f;

namespace RBX
{
	namespace View
	{
		MaterialFactory::MaterialFactory(G3D::TextureManager* textureManager)
		{
			surfacesTexture = new Render::TextureProxy(*textureManager, ContentProvider::singleton().getAssetFile("textures\\surfaces.png"), true);
			fileMeshTexture = new Render::TextureProxy(*textureManager, ContentProvider::singleton().getAssetFile("textures\\JohnTex.png"), false);
			megaTexture = new MegaTextureProxy(*textureManager, ContentProvider::singleton().getAssetFile("textures\\SurfacesStrip.png"));
		}

		G3D::ReferenceCountedPointer<Render::Material> MaterialFactory::getMaterial(Attributes attributes)
		{
			G3D::ReferenceCountedPointer<Render::Material> material;

			std::map<Attributes, G3D::WeakReferenceCountedPointer<Render::Material>>::iterator iter = database.find(attributes);
			if (iter != database.end())
			{
				material = iter->second.createStrongPtr();
			}

			if (material.isNull())
			{
				G3D::Color3 color3 = attributes.color.color3();
				G3D::ReferenceCountedPointer<Render::TextureProxy> surfacesTexture = getSurfacesTexture(color3);

				material = new Render::Material;
				if (attributes.transparency == 1.0f)
				{
					material->appendEmptyLevel();
				}
				else
				{
					material->appendLevel(surfacesTexture, G3D::Color3::WHITE, specular, shiny, attributes.reflectance, attributes.transparency);
				}

				database[attributes] = material;
			}

			return material;
		}

		G3D::ReferenceCountedPointer<Render::Material> MaterialFactory::getMegaMaterial(Attributes attributes)
		{
			Attributes filteredAttributes = {BrickColor::defaultColor(), attributes.transparency, attributes.reflectance};

			G3D::ReferenceCountedPointer<Render::Material> material;

			std::map<Attributes, G3D::WeakReferenceCountedPointer<Render::Material>>::iterator iter = megaDatabase.find(filteredAttributes);
			if (iter != megaDatabase.end())
			{
				material = iter->second.createStrongPtr();
			}

			if (material.isNull())
			{
				material = new Render::Material;
				if (attributes.transparency == 1.0f)
				{
					material->appendEmptyLevel();
				}
				else
				{
					material->appendLevel(megaTexture, G3D::Color3::white(), specular, shiny, attributes.reflectance, attributes.transparency);
				}

				megaDatabase[filteredAttributes] = material;
			}

			return material;
		}

		G3D::ReferenceCountedPointer<Render::TextureProxy> MaterialFactory::getSurfacesTexture(G3D::Color3 tint)
		{
			return surfacesTexture->shade(tint, NULL);
		}
	}
}
