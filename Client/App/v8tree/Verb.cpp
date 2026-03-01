#include "v8tree/Verb.h"

namespace RBX
{
	VerbContainer::VerbContainer(VerbContainer* parent) 
		: parent(parent) 
	{
	}

	VerbContainer::~VerbContainer()
	{
		while (verbs.begin() != verbs.end())
		{
			delete verbs.begin()->second;
		}
	}

	void VerbContainer::setVerbParent(VerbContainer* parent)
	{
		this->parent = parent;
	}

	Verb* VerbContainer::getVerb(std::string name)
	{
		return getVerb(Name::lookup(name));
	}
}
