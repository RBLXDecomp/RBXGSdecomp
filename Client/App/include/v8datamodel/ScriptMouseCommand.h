#pragma once
#include "v8datamodel/MouseCommand.h"

namespace RBX
{
	class Mouse;

	class ScriptMouseCommand : public MouseCommand
	{
	private:
		boost::shared_ptr<Mouse> mouse;

	public:
		ScriptMouseCommand(Workspace* workspace);
		virtual ~ScriptMouseCommand();

		boost::shared_ptr<Mouse> getMouse()
		{
			return mouse;
		}

		virtual TextureId getCursorId() const;
		virtual MouseCommand* onMouseDown(const UIEvent& uiEvent);
		virtual void onMouseHover(const UIEvent& uiEvent);
		virtual void onMouseIdle(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseUp(const UIEvent& uiEvent);
		virtual MouseCommand* onPeekKeyDown(const UIEvent& uiEvent);
		virtual const Name& getName() const;
	};
}
