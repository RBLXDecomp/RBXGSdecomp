#include "reflection/type.h"
#include <g3d/color3.h>
#include <g3d/vector3.h>
#include <boost/shared_ptr.hpp>
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"

namespace RBX
{
	namespace Reflection
	{
		// TODO: check if type singletons are matching
		template<>
		const Type& Type::singleton<boost::shared_ptr<RBX::Reflection::DescribedBase>>()
		{
			static Type type("Object", typeid(boost::shared_ptr<RBX::Reflection::DescribedBase>));
			return type;
		}

		template<>
		const Type& Type::singleton<boost::shared_ptr<Instance>>()
		{
			static Type type("Instance", typeid(boost::shared_ptr<Instance>));
			return type;
		}

		template<>
		const Type& Type::singleton<boost::shared_ptr<Instances>>()
		{
			static Type type("Objects", typeid(boost::shared_ptr<Instances>));
			return type;
		}

		template<>
		const Type& Type::singleton<int>()
		{
			static Type type("int", typeid(int));
			return type;
		}

		template<>
		const Type& Type::singleton<bool>()
		{
			static Type type("bool", typeid(bool));
			return type;
		}

		template<>
		const Type& Type::singleton<float>()
		{
			static Type type("float", typeid(float));
			return type;
		}

		template<>
		const Type& Type::singleton<double>()
		{
			static Type type("double", typeid(double));
			return type;
		}

		template<>
		const Type& Type::singleton<ContentId>()
		{
			static Type type("ContentId", typeid(ContentId));
			return type;
		}

		template<>
		const Type& Type::singleton<std::string>()
		{
			static Type type("string", typeid(std::string));
			return type;
		}

		template<>
		const Type& Type::singleton<G3D::Vector3>()
		{
			static Type type("Vector3", typeid(G3D::Vector3));
			return type;
		}

		template<>
		const Type& Type::singleton<G3D::Color3>()
		{
			static Type type("Color3", typeid(G3D::Color3));
			return type;
		}

		template<>
		const Type& Type::singleton<std::vector<Value>>()
		{
			static Type type("Table", typeid(std::vector<Value>));
			return type;
		}

		template<>
		const Type& Type::singleton<BrickColor>()
		{
			static Type type("BrickColor", typeid(BrickColor));
			return type;
		}

		template<>
		std::string TypedPropertyDescriptor<std::string>::getStringValue(const DescribedBase* instance) const
		{
			return getValue(instance);
		}

		template<>
		std::string TypedPropertyDescriptor<bool>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<bool>::convertToString(getValue(instance));
		}

		template<>
		std::string TypedPropertyDescriptor<float>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<float>::convertToString(getValue(instance));
		}

		template<>
		std::string TypedPropertyDescriptor<int>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<int>::convertToString(getValue(instance));
		}

		template<>
		std::string TypedPropertyDescriptor<G3D::Vector3>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<G3D::Vector3>::convertToString(getValue(instance));
		}

		template<>
		std::string TypedPropertyDescriptor<G3D::Color3>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<G3D::Color3>::convertToString(getValue(instance));
		}

		template<>
		std::string TypedPropertyDescriptor<ContentId>::getStringValue(const DescribedBase* instance) const
		{
			return StringConverter<ContentId>::convertToString(getValue(instance));
		}

		template<>
		bool TypedPropertyDescriptor<std::string>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			setValue(instance, text);
			return true;
		}

		template<>
		bool TypedPropertyDescriptor<bool>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			bool value;
			if (StringConverter<bool>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		bool TypedPropertyDescriptor<float>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			float value;
			if (StringConverter<float>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		bool TypedPropertyDescriptor<int>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			int value;
			if (StringConverter<int>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		bool TypedPropertyDescriptor<G3D::Vector3>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			G3D::Vector3 value;
			if (StringConverter<G3D::Vector3>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		bool TypedPropertyDescriptor<G3D::Color3>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			G3D::Color3 value;
			if (StringConverter<G3D::Color3>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		bool TypedPropertyDescriptor<ContentId>::setStringValue(DescribedBase* instance, const std::string& text) const
		{
			ContentId value;
			if (StringConverter<ContentId>::convertToValue(text, value))
			{
				setValue(instance, value);
				return true;
			}

			return false;
		}

		template<>
		void TypedPropertyDescriptor<float>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				float value;
				if (element->getValue(value))
				{
					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<float>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			float value = getValue(instance);
			element->setValue(value);
		}

		template<>
		void TypedPropertyDescriptor<bool>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				bool value;
				if (element->getValue(value))
				{
					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<bool>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			bool value = getValue(instance);
			element->setValue(value);
		}

		template<>
		void TypedPropertyDescriptor<BrickColor>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				int value;
				if (element->getValue(value))
				{
					setValue(instance, BrickColor(value));
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<BrickColor>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			element->setValue(getValue(instance).asInt());
		}

		template<>
		void TypedPropertyDescriptor<int>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				int value;
				if (element->getValue(value))
				{
					setValue(instance, value);
				}
			}
		}
		
		template<>
		void TypedPropertyDescriptor<int>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			int value = getValue(instance);
			element->setValue(value);
		}

		template<>
		void TypedPropertyDescriptor<ContentId>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
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
		void TypedPropertyDescriptor<ContentId>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			element->setValue(ContentId(getValue(instance)));
		}

		template<>
		void TypedPropertyDescriptor<std::string>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				std::string value;
				if (element->getValue(value))
				{
					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<std::string>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			element->setValue(getValue(instance));
		}

		template<>
		void TypedPropertyDescriptor<G3D::Vector3>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				if (const XmlElement* X = element->findFirstChildByTag(tag_X))
				{
					const XmlElement* Y = element->findFirstChildByTag(tag_Y);
					const XmlElement* Z = element->findFirstChildByTag(tag_Z);

					G3D::Vector3 value;

					X->getValue(value.x);
					Y->getValue(value.y);
					Z->getValue(value.z);

					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<G3D::Vector3>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			const Name* xName = &tag_X;
			XmlElement* xElement = new XmlElement(*xName);
			element->pushBackChild(xElement);

			const Name* yName = &tag_Y;
			XmlElement* yElement = new XmlElement(*yName);
			element->pushBackChild(yElement);

			const Name* zName = &tag_Z;
			XmlElement* zElement = new XmlElement(*zName);
			element->pushBackChild(zElement);

			G3D::Vector3 value = getValue(instance);

			xElement->setValue(value.x);
			yElement->setValue(value.y);
			zElement->setValue(value.z);
		}

		template<>
		void TypedPropertyDescriptor<G3D::Color3>::readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
		{
			if (!element->isXsiNil())
			{
				unsigned int colorValue;
				if (element->getValue(colorValue))
				{
					G3D::Color3 color = G3D::Color3(G3D::Color3uint8::fromARGB(colorValue));
					setValue(instance, color);	
				}
				else
				{
					const XmlElement* R = element->findFirstChildByTag(tag_R);
					const XmlElement* G = element->findFirstChildByTag(tag_G);
					const XmlElement* B = element->findFirstChildByTag(tag_B);

					G3D::Color3 value;

					R->getValue(value.r);
					G->getValue(value.g);
					B->getValue(value.b);

					setValue(instance, value);
				}
			}
		}

		template<>
		void TypedPropertyDescriptor<G3D::Color3>::writeValue(const DescribedBase* instance, XmlElement* element) const
		{
			G3D::Color3uint8 value = G3D::Color3uint8(getValue(instance));
			element->setValue(value.asUInt32());
		}
	}
}
