#include "v8datamodel/ScriptMouseCommand.h"
#include "v8datamodel/Mouse.h"

namespace RBX
{
	ScriptMouseCommand::ScriptMouseCommand(Workspace* workspace)
		: MouseCommand(workspace)
	{
		mouse = Creatable<Instance>::create<Mouse>();
		mouse->setCommand(this);
	}

	ScriptMouseCommand::~ScriptMouseCommand()
	{
		mouse->setCommand(NULL);
		mouse->disconnect_all_slots();
	}

	TextureId ScriptMouseCommand::getCursorId() const
	{
		return mouse->getIcon();
	}

	MouseCommand* ScriptMouseCommand::onMouseDown(const UIEvent& uiEvent)
	{
		mouse->update(uiEvent);
		return this;
	}

	MouseCommand* ScriptMouseCommand::onMouseUp(const UIEvent& uiEvent)
	{
		mouse->update(uiEvent);
		return this;
	}

	MouseCommand* ScriptMouseCommand::onPeekKeyDown(const UIEvent& uiEvent)
	{
		mouse->update(uiEvent);
		return this;
	}

	void ScriptMouseCommand::onMouseIdle(const UIEvent& uiEvent)
	{
		mouse->update(uiEvent);
	}

	void ScriptMouseCommand::onMouseHover(const UIEvent& uiEvent)
	{
		mouse->update(uiEvent);
	}
}
