#include "RenderLib/Material.h"

namespace RBX
{
	namespace Render
	{
		Material::Level::Level(G3D::ReferenceCountedPointer<TextureProxy> baseTexture, G3D::Color3 color, float specular, float shiny, float reflect, float transparent)
			: emptyLevel(false),
			mBaseTexture(baseTexture),
			mColor(color),
			mSpecular(specular),
			mShiny(shiny),
			mReflect(reflect),
			mTransparent(transparent)
		{
			if (baseTexture.notNull())
				mMatte = baseTexture->alphaOnlyVersion();

			if (mTransparent > 0.0f)
				mReflect = 0.0f;
		}

		Material::Level::Level()
			: emptyLevel(true),
			mColor(G3D::Color3::white()),
			mSpecular(0.0f),
			mShiny(0.0f),
			mReflect(0.0f),
			mTransparent(0.0f)
		{
		}

		Material::Level::~Level() {}

		void Material::Level::configureRenderDevice(G3D::RenderDevice* renderDevice) const
		{
			renderDevice->setColor(mColor);
			renderDevice->setSpecularCoefficient(mSpecular);
			renderDevice->setShininess(mShiny);
			renderDevice->setTexture(0, baseTexture(renderDevice));
		}

		Material::Material() {}
		Material::~Material() {}

		const Material::Level& Material::appendEmptyLevel()
		{
			return levels.next();
		}

		const Material::Level& Material::appendLevel(G3D::ReferenceCountedPointer<TextureProxy> baseTexture, G3D::Color3 color, float specular, float shiny, float reflect, float transparent)
		{
			levels.push_back(Material::Level(baseTexture, color, specular, shiny, reflect, transparent));
			return levels.last();
		}

		bool Material::veryTransparent() const
		{
			return levels.size() > 0 && levels.front().mTransparent > 0.25f;
		}
	}
}
