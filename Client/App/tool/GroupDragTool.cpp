#include "tool/GroupDragTool.h"
#include "tool/Dragger.h"
#include "util/Math.h"

namespace RBX
{
	GroupDragTool::GroupDragTool(PartInstance* mousePart, const G3D::Vector3& hitPointWorld, const std::vector<boost::weak_ptr<PartInstance>>& partArray, Workspace* workspace)
		: Base(workspace),
		  dragging(false)
	{
		RBXASSERT(mousePart);
		megaDragger.reset(new MegaDragger(mousePart, partArray, (RootInstance*)workspace));
	}

	GroupDragTool::~GroupDragTool()
	{
		if (dragging)
			RBXASSERT(0);
	}

	const std::string GroupDragTool::getCursorName() const
	{
		return dragging ? "GrabRotateCursor" : "DragCursor";
	}

	MouseCommand* GroupDragTool::onKeyDown(const UIEvent& uiEvent)
	{
		if (dragging)
		{
			switch (uiEvent.key)
			{
			case SDLK_r:
				megaDragger->safeRotate(Math::matrixRotateY());
				break;
			case SDLK_t:
				megaDragger->safeRotate(Math::matrixTiltZ());
				break;
			}

			return this;
		}
		else
		{
			RBXASSERT(0);
			return NULL;
		}
	}

	MouseCommand* GroupDragTool::onMouseDown(const UIEvent& uiEvent)
	{
		downPoint = uiEvent.mousePosition;
		capture();

		return this;
	}

	void GroupDragTool::onMouseIdle(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		G3D::Vector3 hit;

		if (captured() && dragging && megaDragger->mousePartAlive() &&
			Math::intersectRayPlane(getSearchRay(uiEvent), G3D::Plane(G3D::Vector3::unitY(), G3D::Vector3::zero()), hit))
		{
			hit = megaDragger->hitObjectOrPlane(uiEvent);
			megaDragger->continueDragging();

			G3D::Vector3 hitSnapped = Dragger::toGrid(hit);
			G3D::Vector3 delta = Dragger::toGrid(hitSnapped - lastHit);
			lastHit = hitSnapped;

			megaDragger->safeMoveYDrop(delta);
		}
	}

	void GroupDragTool::onMouseMove(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		if (!dragging && (G3D::Vector2(uiEvent.mousePosition) - downPoint).length() > 4.0f)
		{
			dragging = true;

			megaDragger->startDragging();
			megaDragger->alignAndCleanParts();
			lastHit = megaDragger->hitObjectOrPlane(uiEvent);
		}

		onMouseIdle(uiEvent);
	}

	MouseCommand* GroupDragTool::onMouseUp(const UIEvent& uiEvent)
	{
		RBXASSERT(captured());

		if (dragging)
		{
			megaDragger->finishDragging();
			dragging = false;
		}

		releaseCapture();
		return NULL;
	}
}
