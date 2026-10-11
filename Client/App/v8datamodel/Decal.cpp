#include "v8datamodel/Decal.h"
#include "util/TextureId.h"
#include "util/Name.h"
#include "util/ContentProvider.h"

namespace RBX 
{
	char* sDecal = "Decal";

	const Reflection::PropDescriptor<Decal, TextureId> Decal::prop_Texture("Texture", "Appearance", &Decal::getTexture, &Decal::setTexture, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Decal, float> Decal::prop_Specular("Specular", "Appearance", &Decal::getSpecular, &Decal::setSpecular, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Decal, float> Decal::prop_Shiny("Shiny", "Appearance", &Decal::getShiny, &Decal::setShiny, Reflection::PropertyDescriptor::STANDARD);

	Decal::Decal()
		: specular(0.0f),
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

	char* sTexture = "Texture";

	const Reflection::PropDescriptor<Texture, float> Texture::prop_StudsPerTileU("StudsPerTileU", "Appearance", &Texture::getStudsPerTileU, &Texture::setStudsPerTileU, Reflection::PropertyDescriptor::STANDARD);
	const Reflection::PropDescriptor<Texture, float> Texture::prop_StudsPerTileV("StudsPerTileV", "Appearance", &Texture::getStudsPerTileV, &Texture::setStudsPerTileV, Reflection::PropertyDescriptor::STANDARD);

	Texture::Texture()
		: studsPerTile(2.0f, 2.0f)
	{
		setName("Texture");
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

	template<>
	static std::string StringConverter<TextureId>::convertToString(const TextureId& value)
	{
		return value.toString();
	}

	template<>
	static bool StringConverter<TextureId>::convertToValue(const std::string& text, TextureId& value)
	{
		value = TextureId(text);
		return true;
	}

	namespace Reflection
	{
		template<>
		const Type& Type::singleton<TextureId>()
		{
			return Type::singleton<ContentId>();
		}

		template<>
		void TypedPropertyDescriptor<TextureId>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				ContentId value;
				if (element->getValue(value))
				{
					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<TextureId>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			element->setValue(TextureId(getValue(instance)));
		}

		template<>
		bool TypedPropertyDescriptor<TextureId>::hasStringValue() const
		{
			return true;
		}

		template<>
		std::string TypedPropertyDescriptor<TextureId>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<TextureId>::convertToString(getValue(instance));
		}

		template<>
		bool TypedPropertyDescriptor<TextureId>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			TextureId value;
			if (StringConverter<TextureId>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}
	}
}
