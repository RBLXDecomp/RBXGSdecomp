#include "ClientPhysics.h"
#include "NetworkSettings.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	namespace Network
	{
		ClientPhysics::ClientPhysics(PartInstance* part)
			: part(part)
		{
		}

		void ClientPhysics::onEvent(const RunService* source, Heartbeat event)
		{
			G3D::RealTime currentTime = G3D::System::getLocalTime();

			if (NetworkSettings::singleton().clientPhysicsLifetime + lastUpdateTime < currentTime)
				sleep();
			else
				RBXASSERT(0);
		}
	}
}
