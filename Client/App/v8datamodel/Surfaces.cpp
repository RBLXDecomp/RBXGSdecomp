#include "v8datamodel/Surfaces.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
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
		return Bottom.getSurfaceType() == INLET && Front.getSurfaceType() == NO_SURFACE && Back.getSurfaceType() == NO_SURFACE 
			   && Left.getSurfaceType() == NO_SURFACE && Right.getSurfaceType() == NO_SURFACE;
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