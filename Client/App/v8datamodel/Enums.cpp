#include "reflection/enumconverter.h"
#include "AppDraw/Part.h"

namespace RBX
{
	namespace Reflection
	{
		EnumDesc<Part::PartType>::EnumDesc()
			: EnumDescriptor("PartType", typeid(Part::PartType))
		{
			addPair(Part::BALL_PART, "Ball");
			addPair(Part::BLOCK_PART, "Block");
			addPair(Part::CYLINDER_PART, "Cylinder");
		}
	}
}