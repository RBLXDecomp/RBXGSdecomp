#include "tool/ToolsArrow.h"
#include "tool/DragTool.h"
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

	MouseCommand* ArrowToolBase::onMouseDown(const UIEvent& uiEvent)
	{
		RBXASSERT(uiEvent.eventType == UIEvent::MOUSE_LEFT_BUTTON_DOWN);

		G3D::Vector3 hitWorld;

		PartInstance* part = getUnlockedPart(uiEvent, hitWorld);
		if (part)
		{
			Instance* top = getTopSelectable3d(part);
			UserInputBase* input = uiEvent.userInput;

			ServiceClient<Selection> selection(workspace);

			if (!input->keyDown(SDLK_RSHIFT) && !input->keyDown(SDLK_LSHIFT) &&
				!input->keyDown(SDLK_RCTRL) && !input->keyDown(SDLK_LCTRL))
			{
				if (!selection->isSelected(top))
				{
					selection->setSelection(top);
				}

				if (uiEvent.eventType == UIEvent::MOUSE_LEFT_BUTTON_DOWN)
				{
					ServiceClient<FilteredSelection<Instance>> instanceSelection(workspace);
					return DragTool::onMouseDown(part, hitWorld, instanceSelection->items(), uiEvent, workspace);
				}
			}
			else
			{
				if (selection->isSelected(top))
					selection->removeFromSelection(top);
				else
					selection->addToSelection(top);
			}

			return NULL;
		}
		else
		{
			MouseCommand* newCommand = new BoxSelectCommand(workspace);
			return newCommand->onMouseDown(uiEvent);
		}
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
			ILocation* location = workspace->queryTypedChild<ILocation>((int)i);
			ISelectable3d* selectable3d = workspace->queryTypedChild<ISelectable3d>((int)i);

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
