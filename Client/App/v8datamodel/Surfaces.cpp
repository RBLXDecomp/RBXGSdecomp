#include "v8datamodel/Surfaces.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	template<NormalId id>
	SurfaceDescriptor<id>::SurfaceDescriptor(const char* name)
		: PropertyDescriptor(PartInstance::classDescriptor(), Reflection::Type::singleton<Surface>(), name, "Data", LEGACY)
	{
	}

	template<NormalId id>
	bool SurfaceDescriptor<id>::equalValues(const Reflection::DescribedBase* a, const Reflection::DescribedBase* b) const
	{
		return false;
	}

	template<NormalId id>
	void SurfaceDescriptor<id>::readValue(Reflection::DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
	{
		((PartInstance*) instance)->getSurfaces()[id].readValue(element, binder);
	}

	template<NormalId id, typename Type, typename GetFunction, typename SetFunction>
	Type SurfaceGetSet<id, Type, GetFunction, SetFunction>::getValue(const Reflection::DescribedBase* instance) const
	{
		return (((const PartInstance*) instance)->getSurfaces()[id].*get)();
	}

	template<NormalId id, typename Type, typename GetFunction, typename SetFunction>
	void SurfaceGetSet<id, Type, GetFunction, SetFunction>::setValue(Reflection::DescribedBase* instance, const Type& value) const
	{
		(((PartInstance*) instance)->getSurfaces()[id].*set)(value);
	}

	template<NormalId id, typename Type>
	template<typename GetFunction, typename SetFunction>
	SurfacePropDescriptor<id, Type>::SurfacePropDescriptor(const char* name, const char* category, GetFunction get, SetFunction set, Functionality flags)
		: Reflection::TypedPropertyDescriptor<Type>::TypedPropertyDescriptor(PartInstance::classDescriptor(), name, category, 
		  std::auto_ptr<Reflection::TypedPropertyDescriptor<Type>::GetSet>(new SurfaceGetSet<id, Type, GetFunction, SetFunction>(get, set)), flags)
	{
	}

	template<NormalId id, typename Enum>
	template<typename GetFunction, typename SetFunction>
	SurfaceEnumPropDescriptor<id, Enum>::SurfaceEnumPropDescriptor(const char* name, const char* category, GetFunction get, SetFunction set, Functionality flags)
		: EnumPropertyDescriptor(PartInstance::classDescriptor(), Reflection::EnumDesc<Enum>::singleton(), name, category, flags),
		  getset(new SurfaceGetSet<id, Enum, GetFunction, SetFunction>(get, set))
	{
	}

	template<NormalId id, typename Enum>
	bool SurfaceEnumPropDescriptor<id, Enum>::setEnumValue(Reflection::DescribedBase* instance, int intValue) const
	{
		if (Reflection::EnumDesc<Enum>::singleton().isValue(intValue))
		{
			setValue(instance, (Enum) intValue);
			return true;
		}

		return false;
	}

	template<NormalId id, typename Enum>
	size_t SurfaceEnumPropDescriptor<id, Enum>::getIndexValue(const Reflection::DescribedBase* instance) const
	{
		return Reflection::EnumDesc<Enum>::singleton().convertToIndex(getValue(instance));
	}

	template<NormalId id, typename Enum>
	bool SurfaceEnumPropDescriptor<id, Enum>::setIndexValue(Reflection::DescribedBase* instance, size_t index) const
	{
		Enum val;
		if (Reflection::EnumDesc<Enum>::singleton().convertToValue(index, val))
		{
			setValue(instance, val);
			return true;
		}
		
		return false;
	}

	template<NormalId id, typename Enum>
	std::string SurfaceEnumPropDescriptor<id, Enum>::getStringValue(const Reflection::DescribedBase* instance) const
	{
		return Reflection::EnumDesc<Enum>::singleton().convertToString(getValue(instance));
	}

	template<NormalId id, typename Enum>
	bool SurfaceEnumPropDescriptor<id, Enum>::setStringValue(Reflection::DescribedBase* instance, const Name& name) const
	{
		Enum val;
		if (Reflection::EnumDesc<Enum>::singleton().convertToValue(name, val))
		{
			setValue(instance, val);
			return true;
		}

		return false;
	}

	template<NormalId id, typename Enum>
	bool SurfaceEnumPropDescriptor<id, Enum>::setStringValue(Reflection::DescribedBase* instance, const std::string& text) const
	{
		Enum val;
		if (Reflection::EnumDesc<Enum>::singleton().convertToValue(text, val))
		{
			setValue(instance, val);
			return true;
		}

		return false;
	}

	template<NormalId id, typename Enum>
	void SurfaceEnumPropDescriptor<id, Enum>::readValue(Reflection::DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
	{
		if (!element->isXsiNil())
		{
			if (element->isValueType<std::string>())
			{
				std::string text;
				if (element->getValue(text))
				{
					Enum val;
					if (Reflection::EnumDesc<Enum>::singleton().convertToValue(text, val))
					{
						setValue(instance, val);
						return;
					}
				}
			}

			int value;
			if (element->getValue(value))
			{
				setValue(instance, (Enum) value);
			}
			else
			{
				RBXASSERT(false);
			}
		}
	}

	template<NormalId id, typename Enum>
	void SurfaceEnumPropDescriptor<id, Enum>::writeValue(const Reflection::DescribedBase* instance, XmlElement* element) const
	{
		element->setValue(getValue(instance));
	}

	template<NormalId id, typename Enum>
	Enum SurfaceEnumPropDescriptor<id, Enum>::getValue(const Reflection::DescribedBase* object) const
	{
		return getset->getValue(object);
	}

	template<NormalId id, typename Enum>
	void SurfaceEnumPropDescriptor<id, Enum>::setValue(Reflection::DescribedBase* object, const Enum& value) const
	{
		getset->setValue(object, value);
	}

	SurfaceDescriptor<NORM_X> desc_LegacyRight("Front");
	SurfaceDescriptor<NORM_Y> desc_LegacyTop("Top");
	SurfaceDescriptor<NORM_Z> desc_LegacyBack("Left");
	SurfaceDescriptor<NORM_X_NEG> desc_LegacyLeft("Back");
	SurfaceDescriptor<NORM_Y_NEG> desc_LegacyBottom("Bottom");
	SurfaceDescriptor<NORM_Z_NEG> desc_LegacyFront("Right");
	
	SurfaceEnumPropDescriptor<NORM_X, SurfaceType> desc_RightType("RightSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Y, SurfaceType> desc_TopType("TopSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Z, SurfaceType> desc_BackType("BackSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_X_NEG, SurfaceType> desc_LeftType("LeftSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Y_NEG, SurfaceType> desc_BottomType("BottomSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Z_NEG, SurfaceType> desc_FrontType("FrontSurface", "Surface", &Surface::getSurfaceType, &Surface::setSurfaceType, Reflection::PropertyDescriptor::STANDARD);

	SurfaceEnumPropDescriptor<NORM_X, Controller::InputType> desc_RightSurfaceInput("RightSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Y, Controller::InputType> desc_TopSurfaceInput("TopSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Z, Controller::InputType> desc_BackSurfaceInput("BackSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_X_NEG, Controller::InputType> desc_LeftSurfaceInput("LeftSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Y_NEG, Controller::InputType> desc_BottomSurfaceInput("BottomSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);
	SurfaceEnumPropDescriptor<NORM_Z_NEG, Controller::InputType> desc_FrontSurfaceInput("FrontSurfaceInput", "Surface Inputs", &Surface::getInput, &Surface::setSurfaceInput, Reflection::PropertyDescriptor::STANDARD);

	SurfacePropDescriptor<NORM_X, float> desc_RightParamA("RightParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Y, float> desc_TopParamA("TopParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Z, float> desc_BackParamA("BackParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_X_NEG, float> desc_LeftParamA("LeftParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Y_NEG, float> desc_BottomParamA("BottomParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Z_NEG, float> desc_FrontParamA("FrontParamA", "Surface Inputs", &Surface::getParamA, &Surface::setParamA, Reflection::PropertyDescriptor::STANDARD);

	SurfacePropDescriptor<NORM_X, float> desc_RightParamB("RightParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Y, float> desc_TopParamB("TopParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Z, float> desc_BackParamB("BackParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_X_NEG, float> desc_LeftParamB("LeftParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Y_NEG, float> desc_BottomParamB("BottomParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);
	SurfacePropDescriptor<NORM_Z_NEG, float> desc_FrontParamB("FrontParamB", "Surface Inputs", &Surface::getParamB, &Surface::setParamB, Reflection::PropertyDescriptor::STANDARD);

	Surfaces::Surfaces(PartInstance* container)
		: Top(container, NORM_Y),
		  Bottom(container, NORM_Y_NEG),
		  Left(container, NORM_X_NEG),
		  Right(container, NORM_X),
		  Front(container, NORM_Z_NEG),
		  Back(container, NORM_Z)
	{
	}

	
	const Reflection::PropertyDescriptor& Surfaces::getSurfaceType(NormalId face) const
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return desc_BottomType;
		case NORM_Z:
			return desc_BackType;
		case NORM_Z_NEG:
			return desc_FrontType;
		case NORM_X:
			return desc_RightType;
		case NORM_X_NEG:
			return desc_LeftType;
		case NORM_Y:
			return desc_TopType;
		default:
			RBXASSERT(false);
			break;
		}

		return desc_TopType;
	}

	const Reflection::PropertyDescriptor& Surfaces::getSurfaceInput(NormalId face) const
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return desc_BottomSurfaceInput;
		case NORM_Z:
			return desc_BackSurfaceInput;
		case NORM_Z_NEG:
			return desc_FrontSurfaceInput;
		case NORM_X:
			return desc_RightSurfaceInput;
		case NORM_X_NEG:
			return desc_LeftSurfaceInput;
		case NORM_Y:
			return desc_TopSurfaceInput;
		default:
			RBXASSERT(false);
			break;
		}

		return desc_TopSurfaceInput;
	}

	const Reflection::PropertyDescriptor& Surfaces::getParamA(NormalId face) const
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return desc_BottomParamA;
		case NORM_Z:
			return desc_BackParamA;
		case NORM_Z_NEG:
			return desc_FrontParamA;
		case NORM_X:
			return desc_RightParamA;
		case NORM_X_NEG:
			return desc_LeftParamA;
		case NORM_Y:
			return desc_TopParamA;
		default:
			RBXASSERT(false);
			break;
		}

		return desc_TopParamA;
	}

	const Reflection::PropertyDescriptor& Surfaces::getParamB(NormalId face) const
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return desc_BottomParamB;
		case NORM_Z:
			return desc_BackParamB;
		case NORM_Z_NEG:
			return desc_FrontParamB;
		case NORM_X:
			return desc_RightParamB;
		case NORM_X_NEG:
			return desc_LeftParamB;
		case NORM_Y:
			return desc_TopParamB; 
		default:
			RBXASSERT(false);
			break;
		}

		return desc_TopParamB;
	}

	const Surface& Surfaces::operator[](NormalId face) const
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return Bottom;
		case NORM_Z:
			return Back;
		case NORM_Z_NEG:
			return Front;
		case NORM_X:
			return Right;
		case NORM_X_NEG:
			return Left;
		case NORM_Y:
			return Top;
		default:
			RBXASSERT(false);
			break;
		}

		return Top;
	}

	Surface& Surfaces::operator[](NormalId face)
	{
		switch (face)
		{
		case NORM_Y_NEG:
			return Bottom;
		case NORM_Z:
			return Back;
		case NORM_Z_NEG:
			return Front;
		case NORM_X:
			return Right;
		case NORM_X_NEG:
			return Left;
		case NORM_Y:
			return Top;
		default:
			RBXASSERT(false);
			break;
		}

		return Top;
	}

	const bool Surfaces::isStandardPart() const
	{
		return 
			Bottom.getSurfaceType() == INLET && 
			Front.getSurfaceType() == NO_SURFACE && 
			Back.getSurfaceType() == NO_SURFACE && 
			Left.getSurfaceType() == NO_SURFACE && 
			Right.getSurfaceType() == NO_SURFACE;
	}

	bool Surfaces::isSurfaceDescriptor(const Reflection::PropertyDescriptor& desc)
	{
		if (desc == desc_TopType)
		{
			return true;
		}
		else if (desc == desc_BottomType)
		{
			return true;
		}
		else if (desc == desc_LeftType)
		{
			return true;
		}
		else if (desc == desc_RightType)
		{
			return true;
		}
		else if (desc == desc_FrontType)
		{
			return true;
		}
		else if (desc == desc_BackType)
		{
			return true;
		}

		return false;
	}
}