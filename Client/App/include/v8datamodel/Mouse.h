#pragma once
#include "v8tree/Instance.h"
#include "util/UIEvent.h"
#include "util/TextureId.h"
#include <G3D/CoordinateFrame.h>

namespace RBX
{
	class MouseCommand;
	class PartInstance;

	extern const char* sMouse;

	class Mouse : public DescribedNonCreatable<Mouse, Instance, &sMouse>
	{
	private:
		UIEvent lastEvent;
		TextureId icon;
		MouseCommand* command;

	private:
		void cacheUIEvent(const UIEvent& uiEvent)
		{
			lastEvent = uiEvent;
			lastEvent.eventType = UIEvent::NO_EVENT;
		}

		void checkActive() const
		{
			if (!command)
			{
				RBXASSERT(0);
				throw std::runtime_error("This Mouse is no longer active");
			}
		}
	public:
		Mouse()
			: command(NULL)
		{
		}

		void update(const UIEvent&);
		G3D::CoordinateFrame getHit() const;
		G3D::CoordinateFrame getOrigin() const;
		PartInstance* getTarget() const;

		void setCommand(MouseCommand* value)
		{
			command = value;
		}

		TextureId getIcon() const
		{
			checkActive();
			return icon;
		}

		void setIcon(const TextureId& value)
		{
			icon = value;
		}

		int getX() const
		{
			checkActive();
			return lastEvent.mousePosition.x;
		}

		int getY() const
		{
			checkActive();
			return lastEvent.mousePosition.y;
		}

		int getViewSizeX() const
		{
			checkActive();
			return lastEvent.windowSize.x;
		}

		int getViewSizeY() const
		{
			checkActive();
			return lastEvent.windowSize.y;
		}
	};
}
