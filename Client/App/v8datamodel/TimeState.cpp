#include "v8datamodel/TimeState.h"

namespace RBX
{
	TimeState::TimeState()
		: totalVirtualTime(0.0f)
	{
	}

	void TimeState::clear()
	{
		totalVirtualTime = 0.0f;
	}
}
