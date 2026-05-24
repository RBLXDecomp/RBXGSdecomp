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
		ScriptMouseCommand(Workspace*);
		virtual ~ScriptMouseCommand();
		boost::shared_ptr<Mouse> getMouse();
		virtual TextureId getCursorId() const;
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual void onMouseHover(const UIEvent&);
		virtual void onMouseIdle(const UIEvent&);
		virtual MouseCommand* onMouseUp(const UIEvent&);
		virtual MouseCommand* onPeekKeyDown(const UIEvent&);
		virtual const Name& getName() const;
	};
}
