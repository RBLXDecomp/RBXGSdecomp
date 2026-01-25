#include "util/Debug.h"
#include <typeinfo>

bool doCrash = true;

namespace RBX
{
	Debugable::AssertAction Debugable::assertAction = IgnoreAssert;

	void Debugable::dump(std::ostream& stream)
	{
		const std::type_info& type = typeid(*this);
		stream << type.name();
	}

	void Debugable::doCrash()
	{
		if (::doCrash)
			RBXCRASH();
	}
}
