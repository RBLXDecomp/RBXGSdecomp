#include "v8datamodel/Decal.h"
#include "util/TextureId.h"
#include "util/Name.h"
#include "util/ContentProvider.h"

namespace RBX 
{
	const char* sDecal = "Decal";

	Decal::Decal()
		:texture(),
		specular(),
		shiny(20.0f)
	{
		setName("Decal");
	}
	
	float Decal::getShiny() const 
	{
		return shiny;
	}

	float Decal::getSpecular() const 
	{
		return specular;
	};

	TextureId Decal::getTexture() const 
	{
		return texture;
	}

	void Decal::setTexture(TextureId value) 
	{
		if (texture != value) 
		{
			texture = value;
			raisePropertyChanged(prop_Texture);
		}
	}

	void Decal::setSpecular(float value) 
	{
		if (specular != value && value >= 0.0f) 
		{
			specular = value;
			raisePropertyChanged(prop_Specular);
		}
	}

	void Decal::setShiny(float value) 
	{
		if (shiny != value && value > 0.0f) 
		{
			shiny = value;
			raisePropertyChanged(prop_Shiny);
		}
	}
};