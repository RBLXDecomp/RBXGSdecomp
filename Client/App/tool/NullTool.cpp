#include "tool/NullTool.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/UserController.h"
#include "Network/Players.h"

namespace RBX
{
	NewNullTool::NewNullTool(Workspace* workspace)
		: Base(workspace),
		  cursor("ArrowCursor"),
		  hasWaypoint(false)
	{
	}

	NewNullTool::~NewNullTool() {}

	void NewNullTool::onMouseIdle(const UIEvent& uiEvent)
	{
		RBXASSERT(uiEvent.eventType == UIEvent::MOUSE_IDLE);

		if (getPartByLocalCharacter(uiEvent, waypoint) && characterCanReach(waypoint))
		{
			cursor = "ArrowCursor";
		}
		else
		{
			cursor = "ArrowFarCursor";
		}
	}

	void NewNullTool::onMouseHover(const UIEvent& uiEvent)
	{
		RBXASSERT(uiEvent.eventType == UIEvent::MOUSE_MOVE);

		if (getPartByLocalCharacter(uiEvent, waypoint) && characterCanReach(waypoint))
		{
			cursor = "ArrowCursor";
			hasWaypoint = true;
		}
		else
		{
			cursor = "ArrowFarCursor";
			hasWaypoint = false;
		}
	}

	MouseCommand* NewNullTool::onMouseDown(const UIEvent& uiEvent)
	{
		G3D::Vector3 hitPoint;

		PartInstance* part = getPartByLocalCharacter(uiEvent, hitPoint);
		if (part && characterCanReach(hitPoint))
		{
			ModelInstance* character = Network::Players::findLocalCharacter(workspace);
			if (character && !character->contains(part))
			{
				if (Humanoid* humanoid = Humanoid::modelIsCharacter(character))
					humanoid->moveTo(hitPoint, part);
			}
		}

		return this;
	}

	MouseCommand* NewNullTool::onMouseUp(const UIEvent& uiEvent)
	{
		releaseCapture();
		return this;
	}

	void NewNullTool::render3dAdorn(Adorn* adorn)
	{
		if (hasWaypoint)
		{
			if (ControllerService* controllerService = ServiceProvider::find<ControllerService>(workspace))
			{
				const UserInputBase* device = controllerService->getHardwareDevice();

				bool F = device->keyDown(SDLK_UP);
				bool B = device->keyDown(SDLK_DOWN);
				bool L = device->keyDown(SDLK_LEFT);
				bool R = device->keyDown(SDLK_RIGHT);

				bool Ffp = device->keyDown(SDLK_w);
				bool Bfp = device->keyDown(SDLK_s);
				bool Rfp = device->keyDown(SDLK_d);
				bool Lfp = device->keyDown(SDLK_a);

				bool space = device->keyDown(SDLK_SPACE);

				if (!F && !B && !L && !R && !Ffp && !Bfp && !Rfp && !Lfp && !space)
					Humanoid::renderWaypoint(adorn, waypoint);
			}
		}
	}
}
