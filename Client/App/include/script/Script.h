#pragma once
#include "util/Debug.h"

namespace RBX
{
	class Script;
	class ScriptContext;

	class IScriptOwner
	{
	protected:
		virtual IScriptOwner* scriptShouldRun(Script* script) = 0;
		virtual void runScript(Script* script, ScriptContext* context)
		{
			RBXASSERT(0);
		}
		virtual void releaseScript(Script* script)
		{
			RBXASSERT(0);
		}
	public:
		//IScriptOwner(const IScriptOwner&);
		IScriptOwner()
		{
		}
	public:
		//IScriptOwner& operator=(const IScriptOwner&);
	};
}
