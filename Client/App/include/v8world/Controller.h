#pragma once
#include <G3D/ReferenceCount.h>
#include <G3D/Color3.h>

namespace RBX
{
	class Controller : public G3D::ReferenceCountedObject
	{
	public:
		enum ControllerType
		{
			NO_CONTROLLER = 0,
			PRIMARY_CONTROLLER = 1,
			SECONDARY_CONTROLLER = 2,
			PAD_ONE_CONTROLLER = 3,
			PAD_TWO_CONTROLLER = 4,
			AI_CHASE_CONTROLLER = 5,
			AI_FLEE_CONTROLLER = 6,
			PLAYER_CONTROLLER = 7,
			NUM_CONTROLLER_TYPES = 8
		};

		enum InputType
		{
			NO_INPUT = 0,
			LEFT_TRACK_INPUT = 1,
			RIGHT_TRACK_INPUT = 2,
			RIGHT_LEFT_INPUT = 3,
			BACK_FORWARD_INPUT = 4,
			STRAFE_INPUT = 5,
			UP_DOWN_INPUT = 6,
			BUTTON_1_INPUT = 7,
			BUTTON_2_INPUT = 8,
			BUTTON_3_INPUT = 9,
			BUTTON_4_INPUT = 10,
			BUTTON_3_4_INPUT = 11,
			CONSTANT_INPUT = 12,
			SIN_INPUT = 13,
			NUM_INPUT_TYPES = 14,
		};

	public:
		//Controller(const Controller&);
		Controller() {}
		virtual ~Controller() {}
		virtual float getValue(InputType inputType) const { return 0.0f; }
		virtual ControllerType getControllerType() const { return NO_CONTROLLER; }
		bool isNullController();
		virtual bool hasIntelligence() const { return false; }
		virtual bool isUserController() const { return false; }
		//Controller& operator=(const Controller&);

		static bool isControllableInput(InputType inputType); // this is inlined :(
		static G3D::Color3 controllerTypeToColor(ControllerType controllerType);
	};

	class NullController : public Controller
	{
	public:
		virtual ControllerType getControllerType() const
		{
			return NO_CONTROLLER;
		}
		virtual bool hasIntelligence() const
		{
			return false;
		}
		virtual bool isUserController() const
		{
			return false;
		}

		//NullController(const NullController&);
		NullController() : Controller() {}

		//virtual ~NullController();

		//NullController& operator=(const NullController&);

		static NullController* getStaticNullController()
		{
			static NullController n;
			return &n;
		}
	};

	class ControllerTypeArray
	{
	private:
		bool values[Controller::NUM_CONTROLLER_TYPES];
	  
	public:
		void clear()
		{
			for (int i = 0; i < Controller::NUM_CONTROLLER_TYPES; ++i)
				values[i] = false;
		}
	public:
		ControllerTypeArray()
		{
			clear();
		}
	public:
		void setController(Controller::ControllerType controllerType, bool value)
		{
			values[controllerType] = value;
		}
		bool hasController(Controller::ControllerType controllerType) const
		{
			return values[controllerType];
		}

		ControllerTypeArray& operator|=(const ControllerTypeArray&);
	};
}