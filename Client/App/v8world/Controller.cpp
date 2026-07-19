#include "v8world/Controller.h"

namespace RBX
{
	G3D::Color3 Controller::controllerTypeToColor(ControllerType controllerType)
	{
		switch (controllerType)
		{
		case PLAYER_CONTROLLER:
			return G3D::Color3::purple();
		case PRIMARY_CONTROLLER:
			return G3D::Color3::blue();
		case SECONDARY_CONTROLLER:
			return G3D::Color3::red();
		case AI_CHASE_CONTROLLER:
			return G3D::Color3::black();
		case AI_FLEE_CONTROLLER:
			return G3D::Color3::yellow();
		default:
			return G3D::Color3::gray();
		}
	}
}
