#pragma once
#include "v8datamodel/GlobalSettings.h"

namespace RBX
{
	extern const char* sNetworkSettings;

	class NetworkSettings : public GlobalSettingsItem<NetworkSettings, &sNetworkSettings>
	{
	public:
		bool clientPhysics;
		float clientPhysicsSpeed;
		float clientPhysicsLifetime;
		bool printPhysicsErrors;
		bool printInstances;
		bool printProperties;
		bool printPacketBuffer;
		bool logPackets;
		int maxDataModelSendBuffer;
		float dataPacketSize;
		float sendRate;
		int maxSendBPS;
		int minExtraPing;
		int extraPingVariance;

	public:
		NetworkSettings();
	};
}
