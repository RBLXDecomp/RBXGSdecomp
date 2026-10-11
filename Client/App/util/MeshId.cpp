#include "util/MeshId.h"
#include "util/Utilities.h"
#include "reflection/type.h"
#include "reflection/property.h"

namespace RBX
{
	template<>
	static std::string StringConverter<MeshId>::convertToString(const MeshId& value)
	{
		return value.toString();
	}

	template<>
	static bool StringConverter<MeshId>::convertToValue(const std::string& text, MeshId& value)
	{
		value = MeshId(text);
		return true;
	}

	namespace Reflection
	{
		template<>
		const Type& Type::singleton<MeshId>()
		{
			return Type::singleton<ContentId>();
		}

		template<>
		void TypedPropertyDescriptor<MeshId>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
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
		void TypedPropertyDescriptor<MeshId>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			element->setValue(MeshId(getValue(instance)));
		}

		template<>
		bool TypedPropertyDescriptor<MeshId>::hasStringValue() const
		{
			return true;
		}

		template<>
		std::string TypedPropertyDescriptor<MeshId>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<MeshId>::convertToString(getValue(instance));
		}

		template<>
		bool TypedPropertyDescriptor<MeshId>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			MeshId value;
			if (StringConverter<MeshId>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}
	}
}