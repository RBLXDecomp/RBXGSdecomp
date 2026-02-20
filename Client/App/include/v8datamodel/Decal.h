#pragma once

#include "reflection/reflection.h"
#include "v8datamodel/FaceInstance.h"
#include "util/TextureId.h"

namespace RBX 
{
	extern const char* sDecal;

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
		TextureId getTexture() const;
		void setTexture(TextureId);
		float getSpecular() const;
		void setSpecular(float);
		float getShiny() const;
		void setShiny(float);
	};
};