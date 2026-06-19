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
			NO_CONTROLLER,
			PRIMARY_CONTROLLER,
			SECONDARY_CONTROLLER,
			PAD_ONE_CONTROLLER,
			PAD_TWO_CONTROLLER,
			AI_CHASE_CONTROLLER,
			AI_FLEE_CONTROLLER,
			PLAYER_CONTROLLER,
			NUM_CONTROLLER_TYPES
		};

		enum InputType
		{
			NO_INPUT,
			LEFT_TRACK_INPUT,
			RIGHT_TRACK_INPUT,
			RIGHT_LEFT_INPUT,
			BACK_FORWARD_INPUT,
			STRAFE_INPUT,
			UP_DOWN_INPUT,
			BUTTON_1_INPUT,
			BUTTON_2_INPUT,
			BUTTON_3_INPUT,
			BUTTON_4_INPUT,
			BUTTON_3_4_INPUT,
			CONSTANT_INPUT,
			SIN_INPUT,
			NUM_INPUT_TYPES
		};

	public:
		Controller() {}
		virtual ~Controller() {}

		virtual float getValue(InputType inputType) const 
		{ 
			return 0.0f; 
		}

		virtual ControllerType getControllerType() const = 0;
		bool isNullController();
		virtual bool hasIntelligence() const = 0;
		virtual bool isUserController() const = 0;

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

	public:
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