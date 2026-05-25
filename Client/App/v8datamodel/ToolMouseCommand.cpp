#include "v8datamodel/ToolMouseCommand.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Tool.h"
#include "humanoid/Humanoid.h"

namespace RBX
{
	ToolMouseCommand::ToolMouseCommand(Workspace* workspace, Tool* tool)
		: Base(workspace),
		  tool(shared_from(tool))
	{
	}

	ToolMouseCommand::~ToolMouseCommand() {}

	void ToolMouseCommand::onMouseHover(const UIEvent& uiEvent)
	{
		updateTargetPoint(uiEvent);
		ScriptMouseCommand::onMouseHover(uiEvent);
	}

	void ToolMouseCommand::onMouseIdle(const UIEvent& uiEvent)
	{
		updateTargetPoint(uiEvent);
		ScriptMouseCommand::onMouseIdle(uiEvent);
	}

	MouseCommand* ToolMouseCommand::onMouseDown(const UIEvent& uiEvent)
	{
		updateTargetPoint(uiEvent);
		tool->activate();
		return ScriptMouseCommand::onMouseDown(uiEvent);
	}

	MouseCommand* ToolMouseCommand::onMouseUp(const UIEvent& uiEvent)
	{
		updateTargetPoint(uiEvent);
		tool->deactivate();
		return ScriptMouseCommand::onMouseUp(uiEvent);
	}
}
