#pragma once
#include "Network/Players.h"
#include "Replicator.h"

namespace RBX
{
	namespace Network
	{
		extern const char* sServer;

		class Server : public DescribedCreatable<Server, Peer, &sServer>
		{
			class ClientProxy : public Replicator
			{
			private:
				boost::shared_ptr<Player> remotePlayer;
				Server* server;

			public:
				ClientProxy(SystemAddress systemAddress, Server* server);
				virtual ~ClientProxy();
				void sendTop();
				virtual PluginReceiveResult OnReceive(RakPeerInterface*, Packet*);
			protected:
				virtual Player* findTargetPlayer()
				{
					return remotePlayer.get();
				}

				virtual void onSentMarker(long id);
			private:
				virtual bool remoteDeleteOnDisconnect(const Instance* instance) const
				{
					std::vector<Instance*>::const_iterator begin = replicationContainers.begin();
					std::vector<Instance*>::const_iterator end = replicationContainers.end();

					return std::find(begin, end, instance) == end;
				}

				virtual bool canSendItems();
			};

		private:
			int outgoingPort;
			boost::scoped_ptr<worker_thread> pingThread;
			boost::shared_ptr<Players> players;

			static int lastId;
		public:
			static Reflection::SignalDesc<Server, void(std::string, boost::shared_ptr<Instance>)> event_IncommingConnection;

		public:
			Server();
			virtual ~Server();

			void start(int port, int threadSleepTime);
			void stop(int blockDuration);
			int getClientCount();
			virtual XmlElement* write();
			virtual PluginReceiveResult OnReceive(RakPeerInterface*, Packet*);
			void setServerManagerPing(std::string pingUrl, std::string publicIP, int thumbnailId);
		protected:
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
			virtual bool askAddChild(const Instance* instance) const
			{
				return fastDynamicCast<const ClientProxy>(instance) != NULL;
			}
			
		private:
			static worker_thread::work_result ping(boost::weak_ptr<Server>, std::string, int, std::string);
		public:
			static bool serverIsPresent(const Instance* context, bool testInDatamodel);
		};
	}
}
