#include "v8datamodel/GameSettings.h"

namespace RBX
{
	GameSettings::GameSettings()
		: chatHistory(50),
		  chatScrollLength(5),
		  soundEnabled(true),
		  softwareSound(false),
		  animatedCharacter(true)
	{
		setName("Game Options");
	}
}
