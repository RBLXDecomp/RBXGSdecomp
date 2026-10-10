#pragma once
#include <boost/noncopyable.hpp>
#include "v8datamodel/Surface.h"
#include "util/Vector6.h"
#include "reflection/property.h"

namespace RBX
{
	class PartInstance;

	class Surfaces : public boost::noncopyable
	{
	private:
		Surface Top;
		Surface Bottom;
		Surface Left;
		Surface Right;
		Surface Front;
		Surface Back;
	  
	public:
		Surfaces(PartInstance* container);
	public:
		const Surface& operator[](NormalId face) const;
		Surface& operator[](NormalId face);
		Vector6<SurfaceType> surf6() const
		{
			Vector6<SurfaceType> result;

			for (int i = NORM_X; i < NORM_UNDEFINED; i++)
			{
				result[i] = (*this)[(NormalId) i].getSurfaceType();
			}

			return result;
		}
		const Reflection::PropertyDescriptor& getSurfaceType(NormalId face) const;
		const Reflection::PropertyDescriptor& getSurfaceInput(NormalId face) const;
		const Reflection::PropertyDescriptor& getParamA(NormalId face) const;
		const Reflection::PropertyDescriptor& getParamB(NormalId face) const;
		const bool isStandardPart() const;
	  
	public:
		static bool isSurfaceDescriptor(const Reflection::PropertyDescriptor& desc);
	};
}
