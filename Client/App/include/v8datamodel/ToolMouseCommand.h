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
		void updateTargetPoint(const UIEvent&);
	public:
		ToolMouseCommand(Workspace*, Tool*);
		virtual ~ToolMouseCommand();
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual void onMouseHover(const UIEvent&);
		virtual void onMouseIdle(const UIEvent&);
		virtual MouseCommand* onMouseUp(const UIEvent&);
	};
}
