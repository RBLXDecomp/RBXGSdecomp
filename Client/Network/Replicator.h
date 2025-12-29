#pragma once
#include <winsock2.h>
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "util/Events.h"
#include "util/Profiling.h"
#include "util/RunStateOwner.h"
#include <PacketLogger.h>
#include <PluginInterface.h>
#include <RakPeer.h>
#include <RakPeerInterface.h>
#include <boost/scoped_ptr.hpp>

namespace RBX
{
	namespace Network
	{
		extern const char* sPeer;

		class Peer : public Reflection::Described<Peer, &sPeer, Instance>, public PluginInterface, public Listener<RunService, Heartbeat>
		{
		private:
			boost::scoped_ptr<PacketLogger> logger;
			boost::scoped_ptr<Profiling::ThreadProfiler> profilePacketsThread;
		protected:
			const boost::scoped_ptr<RakPeer> rakPeer;
		public:
			//Peer(const Peer&);
		protected:
			Peer();
			virtual ~Peer();
			virtual bool askAddChild(const Instance*) const;
			RakPeerInterface* peerInterface();
			void updateLogger();
			void updateNetworkSimulator();
			virtual void onEvent(const RunService* source, Heartbeat event);
			virtual void Update(RakPeerInterface*);
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		public:
			//Peer& operator=(const Peer&);
		};
	}
}
