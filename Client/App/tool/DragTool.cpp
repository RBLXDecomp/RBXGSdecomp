#include "tool/DragTool.h"
#include "tool/DragUtilities.h"
#include "tool/PartDragTool.h"
#include "tool/GroupDragTool.h"

namespace RBX
{
	MouseCommand* DragTool::onMouseDown(PartInstance* hitPart, const G3D::Vector3& hitWorld, const std::vector<Instance*>& dragInstances, const UIEvent& uiEvent, Workspace* workspace)
	{
		RBXASSERT(hitPart);

		std::vector<boost::weak_ptr<PartInstance>> partArray;
		DragUtilities::instancesToParts(dragInstances, partArray);

		if (partArray.size() == 1)
		{
			RBXASSERT(partArray[0].lock().get() == hitPart);
			MouseCommand* tool = new PartDragTool(hitPart, hitWorld, workspace);
			return tool->onMouseDown(uiEvent);
		}
		else
		{
			MouseCommand* tool = new GroupDragTool(hitPart, hitWorld, partArray, workspace);
			return tool->onMouseDown(uiEvent);
		}
	}
}
