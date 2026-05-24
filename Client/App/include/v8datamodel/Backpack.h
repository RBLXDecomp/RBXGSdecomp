#pragma once
#include "script/Script.h"
#include "v8datamodel/Hopper.h"

namespace RBX
{
	extern const char* sBackpack;
	class Backpack : public RBX::DescribedCreatable<Backpack, Hopper, &sBackpack>,
					 public IScriptOwner
	{
	private:
		virtual IScriptOwner* scriptShouldRun(Script* script);
	public:
		Backpack();
	};
}
