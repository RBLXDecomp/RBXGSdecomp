#include "v8datamodel/Backpack.h"
#include "v8datamodel/Workspace.h"
#include "Network/Players.h"

namespace RBX
{
	const char* const sBackpack = "Backpack";

	Backpack::Backpack()
	{
		setName("Backpack");
	}

	IScriptOwner* Backpack::scriptShouldRun(Script* script)
	{
		RBXASSERT(this->isAncestorOf(script));

		Workspace* workspace = ServiceProvider::find<Workspace>(this);
		if (workspace)
		{
			bool isLocalBackpack = getParent() == Network::Players::findLocalPlayer(this);
			bool isBackendProcessing = Network::Players::backendProcessing(this, true);
			bool isLocalScript = dynamic_cast<LocalScript*>(script) != NULL;
			bool isParentOfHopperBin = dynamic_cast<HopperBin*>(script->getParent()) != NULL;

			if ((!isLocalScript && ((isParentOfHopperBin && isLocalBackpack) || isBackendProcessing)) || 
				(isLocalScript && isLocalBackpack))
			{
				return workspace;
			}
		}

		return NULL;
	}
}
