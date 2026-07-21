#include "v8datamodel/SafeChat.h"

namespace RBX
{
	static void inlinedDelete(ChatOption* c)
	{
		delete c;
	}

	ChatOption::~ChatOption()
	{
		std::for_each(children.begin(), children.end(), &inlinedDelete);
	}
}
