#pragma once
#include "v8datamodel/Hopper.h"
#include "script/Script.h"

namespace RBX
{
	extern const char* sBackpack;
	class Backpack : public RBX::DescribedCreatable<Backpack, Hopper, &sBackpack>,
						  public IScriptOwner
	{
	private:
		virtual IScriptOwner* scriptShouldRun(Script*);
	public:
		//Backpack(const Backpack&);
		Backpack();
		virtual ~Backpack();
	public:
		//Backpack& operator=(const Backpack&);
	};
}
