#include "v8datamodel/Mouse.h"
#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/PartInstance.h"
#include <G3D/Ray.h>

namespace RBX
{
	const char* const sMouse = "Mouse";

	static Reflection::RefPropDescriptor<Mouse, PartInstance> desc_Target("Target", "Data", &Mouse::getTarget, NULL, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::PropDescriptor<Mouse, int> desc_Mouse_X("X", "Data", &Mouse::getX, NULL, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::PropDescriptor<Mouse, int> desc_Mouse_Y("Y", "Data", &Mouse::getY, NULL, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::PropDescriptor<Mouse, int> desc_Mouse_ViewSizeX("ViewSizeX", "Data", &Mouse::getViewSizeX, NULL, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::PropDescriptor<Mouse, int> desc_Mouse_ViewSizeY("ViewSizeY", "Data", &Mouse::getViewSizeY, NULL, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::PropDescriptor<Mouse, TextureId> desc_Mouse_Icon("Icon", "Data", &Mouse::getIcon, &Mouse::setIcon, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::PropDescriptor<Mouse, G3D::CoordinateFrame> desc_Hit("Hit", "Data", &Mouse::getHit, NULL, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::PropDescriptor<Mouse, G3D::CoordinateFrame> desc_Origin("Origin", "Data", &Mouse::getOrigin, NULL, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Move("Move");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Idle("Idle");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Button1Down("Button1Down");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Button2Down("Button2Down");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Button1Up("Button1Up");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_Button2Up("Button2Up");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_WheelForward("WheelForward");
	static Reflection::SignalDesc<Mouse, void(void)> desc_Mouse_WheelBackward("WheelBackward");
	static Reflection::SignalDesc<Mouse, void(std::string)> desc_Mouse_KeyDown("KeyDown", "Key");

	G3D::CoordinateFrame Mouse::getHit() const
	{
		checkActive();

		G3D::CoordinateFrame origin = getOrigin();

		if (command)
			command->getPartByLocalCharacter(lastEvent, origin.translation);

		return origin;
	}

	G3D::CoordinateFrame Mouse::getOrigin() const
	{
		checkActive();

		G3D::Ray ray;

		if (command)
			ray = command->getUnitMouseRay(lastEvent);

		G3D::CoordinateFrame origin(ray.origin);
		origin.lookAt(ray.origin + ray.direction);

		return origin;
	}

	PartInstance* Mouse::getTarget() const
	{
		checkActive();

		if (command)
			return command->getPart(lastEvent, NULL);
		else
			return NULL;
	}

	void Mouse::update(const UIEvent& uiEvent)
	{
		switch (uiEvent.eventType)
		{
		case UIEvent::MOUSE_MOVE:
			cacheUIEvent(uiEvent);
			desc_Mouse_Move.fire(this);
			break;
		case UIEvent::MOUSE_IDLE:
			cacheUIEvent(uiEvent);
			desc_Mouse_Idle.fire(this);
			break;
		case UIEvent::MOUSE_LEFT_BUTTON_DOWN:
			cacheUIEvent(uiEvent);
			desc_Mouse_Button1Down.fire(this);
			break;
		case UIEvent::MOUSE_LEFT_BUTTON_UP:
			cacheUIEvent(uiEvent);
			desc_Mouse_Button1Up.fire(this);
			break;
		case UIEvent::MOUSE_RIGHT_BUTTON_DOWN:
			cacheUIEvent(uiEvent);
			desc_Mouse_Button2Down.fire(this);
			break;
		case UIEvent::MOUSE_RIGHT_BUTTON_UP:
			cacheUIEvent(uiEvent);
			desc_Mouse_Button2Up.fire(this);
			break;
		case UIEvent::MOUSE_WHEEL_FORWARD:
			desc_Mouse_WheelForward.fire(this);
			break;
		case UIEvent::MOUSE_WHEEL_BACKWARD:
			desc_Mouse_WheelBackward.fire(this);
			break;
		case UIEvent::KEY_DOWN:
			desc_Mouse_KeyDown.fire(this, std::string(1, (char)uiEvent.key));
			break;
		}
	}
}
