#include "tool/ToolsArrow.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Selection.h"
#include "v8datamodel/Workspace.h"
#include <G3D/Rect2D.h>

namespace RBX
{
	const std::string ArrowToolBase::getCursorName() const
	{
		return overInstance ? "DragCursor" : "ArrowCursor";
	}

	void ArrowToolBase::onMouseIdle(const UIEvent& uiEvent)
	{
		overInstance = getUnlockedPart(uiEvent) != NULL;
	}

	void ArrowToolBase::onMouseHover(const UIEvent& uiEvent)
	{
		onMouseIdle(uiEvent);
	}

	BoxSelectCommand::BoxSelectCommand(Workspace* workspace)
		: Base(workspace),
		  selection((Instance*)workspace)
	{
	}

	MouseCommand* BoxSelectCommand::onMouseDown(const UIEvent& uiEvent)
	{
		previousItemsInBox.clear();
		capture();

		UserInputBase* input = uiEvent.userInput;
		reverseSelecting = input->keyDown(SDLK_RSHIFT) || input->keyDown(SDLK_LSHIFT);

		if (!reverseSelecting)
			selection->clearSelection();

		mouseDownView = uiEvent.mousePosition;
		mouseCurrentView = uiEvent.mousePosition;
		return this;
	}

	void BoxSelectCommand::onMouseMove(const UIEvent& uiEvent)
	{
		mouseCurrentView = uiEvent.mousePosition;

		std::set<Instance*> instances;
		getMouseInstances(instances, uiEvent, G3D::Rect2D::xyxy(mouseDownView.x, mouseDownView.y, mouseCurrentView.x, mouseCurrentView.y));

		if (reverseSelecting)
			selectReverse(instances);
		else
			selectAnd(instances);
	}

	void BoxSelectCommand::render2d(Adorn* adorn)
	{
		G3D::Rect2D temp = G3D::Rect2D::xyxy(mouseDownView.x, mouseDownView.y, mouseCurrentView.x, mouseCurrentView.y);
		adorn->outlineRect2d(temp, 0.5f, G3D::Color3::gray());
	}

	void BoxSelectCommand::getMouseInstances(std::set<Instance*>& instances, const UIEvent& uiEvent, G3D::Rect2D& selectBox)
	{
		Camera* camera = workspace->getCamera();
		G3D::Rect2D viewport = G3D::Rect2D::xyxy(0.0f, 0.0f, uiEvent.windowSize.x, uiEvent.windowSize.y);

		for (size_t i = 0; i < workspace->numChildren(); i++)
		{
			ILocation* location = Instance::fastDynamicCast<ILocation>(workspace->getChild(i));
			ISelectable3d* selectable3d = Instance::fastDynamicCast<ISelectable3d>(workspace->getChild(i));

			if (location && selectable3d)
			{
				Instance* instance = workspace->getChild(i);
				if (!PartInstance::getLocked(instance))
				{
					G3D::Vector3 gridPos = location->getLocation().translation;
					G3D::Vector3 projection = camera->getGCamera().project(gridPos, viewport);

					if (selectBox.contains(projection.xy()))
						instances.insert(instance);
				}
			}
		}
	}
}
