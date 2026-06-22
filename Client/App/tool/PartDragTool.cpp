#include "tool/PartDragTool.h"
#include "tool/RunDragger.h"
#include "tool/MegaDragger.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Workspace.h"

namespace RBX
{
	PartDragTool::PartDragTool(PartInstance* mousePart, const G3D::Vector3& hitPointWorld, Workspace* workspace)
		: Base(workspace),
		  hitWorld(hitPointWorld),
		  dragging(false)
	{
		RBXASSERT(mousePart);
		runDragger.reset(new RunDragger);

		std::vector<PVInstance*> pvInstances;
		pvInstances.push_back(mousePart);

		megaDragger.reset(new MegaDragger(mousePart, pvInstances, (RootInstance*)workspace));
	}

	PartDragTool::~PartDragTool() {}

	MouseCommand* PartDragTool::onMouseDown(const UIEvent& uiEvent)
	{
		downPoint = uiEvent.mousePosition;
		capture();

		return this;
	}

	void PartDragTool::onMouseIdle(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		if (captured() && dragging && megaDragger->mousePartAlive())
		{
			megaDragger->continueDragging();
			runDragger->snap(getUnitMouseRay(uiEvent));
		}
	}

	void PartDragTool::onMouseDelta(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		if (!dragging && megaDragger->mousePartAlive())
		{
			dragging = true;

			megaDragger->startDragging();
			runDragger->init(workspace, megaDragger->getMousePart(), hitWorld);
		}

		onMouseIdle(uiEvent);
	}

	MouseCommand* PartDragTool::onMouseUp(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		if (dragging)
		{
			if (megaDragger->mousePartAlive())
				runDragger->snap(getUnitMouseRay(uiEvent));

			megaDragger->finishDragging();
			dragging = false;
		}

		releaseCapture();
		return NULL;
	}

	MouseCommand* PartDragTool::onKeyDown(const UIEvent& uiEvent)
	{
		if (megaDragger->mousePartAlive())
		{
			if (dragging)
			{
				switch (uiEvent.key)
				{
				case SDLK_r:
					runDragger->rotatePart(megaDragger->getMousePart().lock().get());
					break;
				case SDLK_t:
					runDragger->tiltPart(megaDragger->getMousePart().lock().get(), workspace->getCamera()->getCameraCoordinateFrame());
					break;
				}
			}

			return this;
		}
		else
		{
			return NULL;
		}
	}

	void PartDragTool::render3dAdorn(Adorn* adorn)
	{
		if (megaDragger->mousePartAlive())
			megaDragger->getMousePart().lock()->render3dSelect(adorn, SELECT_NORMAL);
	}
}
