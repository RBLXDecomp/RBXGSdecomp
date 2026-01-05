#include "security/SecurityContext.h"

namespace RBX
{
	namespace Security
	{
		bool Context::isInRole(Identities identity, Permissions permission)
		{
			switch (identity)
			{
			case Anonymous:
			case GameScript:
				return permission == None;

			case LocalGUI:
			case CmdLine:
			case TrustedCOM:
			case TrustedWebService:
			case Replicator:
				return true;

			default:
				return false;
			}
		}
	}
}