#pragma once
#include "script/Script.h"
#include "v8datamodel/Hopper.h"

namespace RBX
{
	extern const char* sPlayerHopper;
	class Backpack : public RBX::DescribedCreatable<PlayerHopper, Hopper, &sPlayerHopper>,
					 public IScriptOwner
	{
	private:
		virtual IScriptOwner* scriptShouldRun(Script* script);
	public:
		PlayerHopper();
	};
}
