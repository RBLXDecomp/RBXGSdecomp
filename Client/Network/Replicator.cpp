#include <RakPeer.h>
#include <PacketLogger.h>
#include "Replicator.h"
#include <g3d/system.h>

void checkDisconnect(RBX::Instance* instance);

namespace RBX
{
	namespace Network
	{
		void Peer::onEvent(const RunService* source, Heartbeat event)
		{
			double tick = G3D::System::getTick() + event.step / 5.0;

			do
			{
				Packet* receivedPacket = rakPeer->Receive();
				if (receivedPacket)
					rakPeer->DeallocatePacket(receivedPacket);
				else
					break;
			}
			while (G3D::System::getTick() < tick);

			for_eachChild(&checkDisconnect);
		}

		RakPeerInterface* Peer::peerInterface()
		{
			return rakPeer.get();
		}
	}
}
