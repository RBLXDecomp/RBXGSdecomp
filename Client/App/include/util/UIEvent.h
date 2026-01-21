#pragma once
#include "util/UserInputBase.h"
#include <SDL.h>
#include <G3D/Vector2int16.h>

namespace RBX 
{
	class UIEvent
	{
	public:
		enum EventType 
		{
			NO_EVENT,
			MOUSE_RIGHT_BUTTON_DOWN,
			MOUSE_RIGHT_BUTTON_UP,
			MOUSE_LEFT_BUTTON_DOWN,
			MOUSE_LEFT_BUTTON_UP,
			MOUSE_MOVE,
			MOUSE_DELTA,
			MOUSE_IDLE,
			MOUSE_WHEEL_FORWARD,
			MOUSE_WHEEL_BACKWARD,
			KEY_DOWN,
			KEY_UP
		};

	public:
		EventType eventType;
		UserInputBase* userInput;
		union 
		{
			struct 
			{
				G3D::Vector2int16 mousePosition;
				G3D::Vector2int16 mouseDelta;
				G3D::Vector2int16 windowSize;
			};
			struct 
			{
				SDLKey key;
				SDLMod mod;
			};
		};

	public:
		UIEvent(UserInputBase* userInput, EventType eventType, SDLKey key, SDLMod mod) 
			: userInput(userInput),
			  eventType(eventType),
			  key(key),
			  mod(mod)	
		{
		}

		UIEvent(UserInputBase* userInput, EventType eventType, G3D::Vector2int16 mousePosition, G3D::Vector2int16 mouseDelta)
			: userInput(userInput),
			  eventType(eventType),
			  mousePosition(mousePosition),
			  mouseDelta(mouseDelta)	
		{
		}

		UIEvent()
			: userInput(NULL),
			  eventType(NO_EVENT)
		{
		}

		bool isMouseEvent() const
		{
			return eventType == MOUSE_RIGHT_BUTTON_DOWN 
				|| eventType == MOUSE_RIGHT_BUTTON_UP 
				|| eventType == MOUSE_LEFT_BUTTON_DOWN 
				|| eventType == MOUSE_LEFT_BUTTON_UP 
				|| eventType == MOUSE_MOVE 
				|| eventType == MOUSE_DELTA 
				|| eventType == MOUSE_IDLE; 
		}
		bool isKeyPressedEvent(SDLKey) const;
		bool isKeyUpEvent(SDLKey) const;
		bool isKeyPressedWithShiftEvent(SDLKey) const;
		bool isKeyPressedWithShiftEvent() const;
	};
}
