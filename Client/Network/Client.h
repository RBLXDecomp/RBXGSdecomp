#pragma once
#include <winsock2.h>
#include "reflection/signal.h"
#include "v8xml/XmlElement.h"
#include "Replicator.h"
#include <boost/shared_ptr.hpp>
#include <string>

class Exposer : protected PacketLogger
{
public:
	Exposer() : PacketLogger() {}

	static const char* IDTOString(int id);
};

namespace RBX
{
	namespace Network
	{
		extern const char* sClient;

		class Client : public DescribedCreatable<Client, Peer, &sClient>, public Service, public Listener<ServiceProvider, Closing>
		{
		private:
			SystemAddress serverId;
		public:
			static Reflection::SignalDesc<Client, void(std::string, boost::shared_ptr<Instance>)> event_ConnectionAccepted;
			static Reflection::SignalDesc<Client, void(std::string, int)> event_ConnectionFailed;
			static Reflection::SignalDesc<Client, void(std::string)> event_ConnectionRejected;

		public:
			//Client(const Client&);
			Client();
			virtual ~Client();
			void connect(std::string, int, int, int);
			void disconnect(int blockDuration);
			virtual PluginReceiveResult OnReceive(RakPeerInterface*, Packet*);
			virtual XmlElement* write()
			{
				return NULL;
			}
		protected:
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
			virtual void onEvent(const ServiceProvider* source, Closing event);
		public:
			//Client& operator=(const Client&);

			static bool clientIsPresent(const Instance* context, bool testInDatamodel);
		};
	}
}
