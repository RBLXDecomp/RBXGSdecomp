#pragma once
#include "v8datamodel/ScriptMouseCommand.h"

namespace RBX
{
	class Tool;

	extern const char* sToolMouseCommand;

	class ToolMouseCommand : public Named<ScriptMouseCommand, &sToolMouseCommand>
	{
	private:
		boost::shared_ptr<Tool> tool;

	private:
		void updateTargetPoint(const UIEvent& uiEvent);
	public:
		ToolMouseCommand(Workspace* workspace, Tool* tool);
		virtual ~ToolMouseCommand();
		virtual MouseCommand* onMouseDown(const UIEvent& uiEvent);
		virtual void onMouseHover(const UIEvent& uiEvent);
		virtual void onMouseIdle(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseUp(const UIEvent& uiEvent);
	};
}
