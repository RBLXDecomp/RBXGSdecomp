#include "humanoid/Flying.h"

namespace RBX
{
	Flying::Flying(Humanoid* humanoid)
		: Base(humanoid, 300.0f, 50.0f),
		  desiredYRotationalVelocity(0.0f)
	{
	}
}
