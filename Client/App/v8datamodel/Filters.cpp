#include "v8datamodel/Filters.h"
#include "humanoid/Humanoid.h"
#include "Network/Players.h"

namespace RBX
{
	bool Unlocked::unlocked(const Primitive* testMe)
	{
		return !PartInstance::fromPrimitiveConst(testMe)->getPartLocked();
	}

	PartByLocalCharacter::PartByLocalCharacter(Instance* root)
	{
		character = shared_from(Network::Players::findLocalCharacter(root));
		head = shared_from(Humanoid::getHeadFromCharacter(character.get()));
	}

	HitTestFilter::Result PartByLocalCharacter::filterResult(const Primitive* testMe) const
	{
		if (character && head)
		{
			const PartInstance* p = PartInstance::fromPrimitiveConst(testMe);

			if (p->isDescendentOf(character.get()))
			{
				return head->getIsTransparent() ? HitTestFilter::IGNORE_PRIM : HitTestFilter::STOP_TEST;
			}
		}

		return HitTestFilter::INCLUDE_PRIM;
	}
}
