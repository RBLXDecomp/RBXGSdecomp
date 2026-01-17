#pragma once
#include "RenderLib/TextureProxy.h"
#include <G3D/Array.h>

namespace RBX
{
	namespace Render
	{
		class Material : public G3D::ReferenceCountedObject
		{
		public:
			class Level
			{
				friend class Material;

			private:
				bool emptyLevel;
				G3D::ReferenceCountedPointer<TextureProxy> mMatte;
				G3D::ReferenceCountedPointer<TextureProxy> mBaseTexture;
				G3D::Color3 mColor;
				float mSpecular;
				float mShiny;
				float mReflect;
				float mTransparent;

			public:
				//Level(const Level&);
				Level();
			private:
				Level(G3D::ReferenceCountedPointer<TextureProxy> baseTexture, G3D::Color3 color, float specular, float shiny, float reflect, float transparent);
				bool isEmptyLevel() const;
			public:
				~Level();

				G3D::ReferenceCountedPointer<G3D::Texture> baseTexture(G3D::RenderDevice* rd) const
				{
					if (mBaseTexture.isNull())
						return NULL;
					else
						return mBaseTexture->resolve(rd);
				}

				void setBaseTexture(G3D::ReferenceCountedPointer<TextureProxy>);

				G3D::ReferenceCountedPointer<G3D::Texture> matte(G3D::RenderDevice* rd) const
				{
					if (mMatte.isNull())
						return NULL;
					else
						return mMatte->resolve(rd);
				}

				const G3D::Color3& color() const
				{
					return mColor;
				}
				float specular() const;
				float shiny() const;
				float reflect() const
				{
					return mReflect;
				}
				float transparent() const
				{
					return mTransparent;
				}
				void configureRenderDevice(G3D::RenderDevice* renderDevice) const;

				//Level& operator=(const Level&);
			};

		private:
			G3D::Array<Level> levels;
		public:
			//Material(const Material&);
			Material();
			const Level& appendEmptyLevel();
			const Level& appendLevel(G3D::Color3, float, float, float, float);
			const Level& appendLevel(G3D::ReferenceCountedPointer<TextureProxy> baseTexture, G3D::Color3 color, float specular, float shiny, float reflect, float transparent);

			virtual ~Material();
			size_t levelCount() const;

			Level& getLevel(int);

			const Level* detailLevel(float lvl) const
			{
				if (levels.size() == 0)
				{
					return NULL;
				}
				else
				{
					int sizeMinusOne = levels.size() - 1;
					int level = lvl * levels.size();

					if (level <= 0)
						level = 0;
					else if (level >= sizeMinusOne)
						level = sizeMinusOne;

					return !levels[level].emptyLevel ? levels.begin() + level : NULL;
				}
			}

			bool veryTransparent() const;
			//Material& operator=(const Material&);
		};
	}
}
