#pragma once
#include <G3D/Vector3.h>
#include <vector>

namespace RBX
{
	class MouseCommand;
	class PartInstance;
	class Instance;
	class UIEvent;
	class Workspace;

	class DragTool
	{
	public:
		static MouseCommand* onMouseDown(PartInstance* hitPart, const G3D::Vector3& hitWorld, const std::vector<Instance*>& dragInstances, const UIEvent& uiEvent, Workspace* workspace);
	};
}
