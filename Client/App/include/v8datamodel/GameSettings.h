#pragma once
#include "v8datamodel/GlobalSettings.h"

namespace RBX
{
	extern const char* sGameSettings;

	class GameSettings : public GlobalSettingsItem<GameSettings, &sGameSettings>
	{
	public:
		int chatHistory;
		int chatScrollLength;
		bool soundEnabled;
		bool softwareSound;
		bool animatedCharacter;

	public:
		GameSettings();
	};
}
