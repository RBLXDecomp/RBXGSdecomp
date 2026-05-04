#include "v8datamodel/Decal.h"
#include "util/TextureId.h"
#include "util/Name.h"
#include "util/ContentProvider.h"

namespace RBX 
{
	const char* sDecal = "Decal";
	const char* sTexture = "Texture";
	const Reflection::PropDescriptor<Decal, TextureId> Decal::prop_Texture("Texture", "Appearance", &Decal::getTexture, &Decal::setTexture, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Decal, float> Decal::prop_Specular("Specular", "Appearance", &Decal::getSpecular, &Decal::setSpecular, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Decal, float> Decal::prop_Shiny("Shiny", "Appearance", &Decal::getShiny, &Decal::setShiny, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Texture, float> Texture::prop_StudsPerTileU("StudsPerTileU", "Appearance", &Texture::getStudsPerTileU, &Texture::setStudsPerTileU, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Texture, float> Texture::prop_StudsPerTileV("StudsPerTileV", "Appearance", &Texture::getStudsPerTileV, &Texture::setStudsPerTileV, Reflection::PropertyDescriptor::STANDARD);

	Decal::Decal()
		:texture(),
		specular(),
		shiny(20.0f)
	{
		setName("Decal");
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

	void Texture::setStudsPerTileU(float value)
	{
		if (studsPerTile.x != value && value > 0.0f)
		{
			studsPerTile.x = value;
			raisePropertyChanged(prop_StudsPerTileU);
		}
	}

	void Texture::setStudsPerTileV(float value)
	{
		if (studsPerTile.y != value && value > 0.0f)
		{
			studsPerTile.y = value;
			raisePropertyChanged(prop_StudsPerTileV);
		}
	}

	Texture::Texture()
		: studsPerTile(2.0f, 2.0f)
	{
		setName("Texture");
	}
};