#pragma once

#include "reflection/reflection.h"
#include "v8datamodel/FaceInstance.h"
#include "util/TextureId.h"
#include <G3D/Vector2.h>

namespace RBX 
{
	extern const char* sDecal;
	extern const char* sTexture;

	class Decal : public DescribedCreatable<Decal,FaceInstance,&sDecal> 
	{
	private:
		TextureId texture;
		float specular;
		float shiny;
	public:
		static const Reflection::PropDescriptor<Decal, TextureId> prop_Texture;
		static const Reflection::PropDescriptor<Decal, float> prop_Specular;
		static const Reflection::PropDescriptor<Decal, float> prop_Shiny;

		Decal();
		TextureId getTexture() const
		{
			return texture;
		}

		void setTexture(TextureId);

		float getSpecular() const
		{
			return specular;
		}

		void setSpecular(float);

		float getShiny() const
		{
			return shiny;
		}
		void setShiny(float);
	};
	
	class Texture : public DescribedCreatable<Texture, Decal, &sTexture>
	{
	private:
		G3D::Vector2 studsPerTile;
	public:
		static const Reflection::PropDescriptor<Texture, float> prop_StudsPerTileU;
		static const Reflection::PropDescriptor<Texture, float> prop_StudsPerTileV;
		
		Texture();
		const G3D::Vector2& getStudsPerTile()
		{
			return studsPerTile;
		}

		float getStudsPerTileU() const
		{
			return studsPerTile.x;
		}
		void setStudsPerTileU(float value);
		float getStudsPerTileV() const
		{
			return studsPerTile.y;
		}
		void setStudsPerTileV(float value);
	};
};