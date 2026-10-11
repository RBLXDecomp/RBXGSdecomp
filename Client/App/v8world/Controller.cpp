#include "v8world/Controller.h"
#include "reflection/enumconverter.h"

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

	namespace Reflection
	{
		EnumDesc<Controller::InputType>::EnumDesc()
			: EnumDescriptor("InputType", typeid(Controller::InputType))
		{
			addPair(Controller::NO_INPUT, "NoInput");
			addPair(Controller::LEFT_TRACK_INPUT, "LeftTread");
			addPair(Controller::RIGHT_TRACK_INPUT, "RightTread");
			addPair(Controller::RIGHT_LEFT_INPUT, "Steer");
			addPair(Controller::BACK_FORWARD_INPUT, "Throtle");
			addPair(Controller::UP_DOWN_INPUT, "UpDown");
			addPair(Controller::BUTTON_1_INPUT, "Action1");
			addPair(Controller::BUTTON_2_INPUT, "Action2");
			addPair(Controller::BUTTON_3_INPUT, "Action3");
			addPair(Controller::BUTTON_4_INPUT, "Action4");
			addPair(Controller::BUTTON_3_4_INPUT, "Action5");
			addPair(Controller::CONSTANT_INPUT, "Constant");
			addPair(Controller::SIN_INPUT, "Sin");
		}

		EnumDesc<Controller::ControllerType>::EnumDesc()
			: EnumDescriptor("ControllerType", typeid(Controller::ControllerType))
		{
			addPair(Controller::PLAYER_CONTROLLER, "Player");
			addPair(Controller::PRIMARY_CONTROLLER, "KeyboardRight");
			addPair(Controller::SECONDARY_CONTROLLER, "KeyboardLeft");
			addPair(Controller::PAD_ONE_CONTROLLER, "Joypad1");
			addPair(Controller::PAD_TWO_CONTROLLER, "Joypad2");
			addPair(Controller::AI_CHASE_CONTROLLER, "Chase");
			addPair(Controller::AI_FLEE_CONTROLLER, "Flee");
			addPair(Controller::NO_CONTROLLER, "None");
		}
	}
}
