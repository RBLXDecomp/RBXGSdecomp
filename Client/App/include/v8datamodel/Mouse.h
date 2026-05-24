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
		void cacheUIEvent(const UIEvent&);
		void checkActive() const;
	public:
		Mouse();
		void update(const UIEvent&);
		G3D::CoordinateFrame getHit() const;
		G3D::CoordinateFrame getOrigin() const;
		PartInstance* getTarget() const;
		void setCommand(MouseCommand*);
		TextureId getIcon() const;
		void setIcon(const TextureId&);
		int getX() const;
		int getY() const;
		int getViewSizeX() const;
		int getViewSizeY() const;
	};
}
