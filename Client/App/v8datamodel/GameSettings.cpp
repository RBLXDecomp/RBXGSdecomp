#include "v8datamodel/GameSettings.h"

namespace RBX
{
	const char* const sGameSettings = "GameSettings";

	Reflection::BoundProp<bool, 1> prop_AnimateCharacter("AnimatedCharacter", "Control", &GameSettings::animatedCharacter, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<int, 1> prop_ChatHistory("ChatHistory", "Online", &GameSettings::chatHistory, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<int, 1> prop_ChatScrollLength("ChatScrollLength", "Online", &GameSettings::chatScrollLength, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_SoundEnabled("SoundEnabled", "Sound", &GameSettings::soundEnabled, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_SoftwareSound("SoftwareSound", "Sound", &GameSettings::softwareSound, Reflection::PropertyDescriptor::STANDARD);

	GlobalSettingsItem<GameSettings, &sGameSettings>* GameSettings::sing = NULL;

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
