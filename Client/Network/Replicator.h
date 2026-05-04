#pragma once
#include <PluginInterface.h>
#include <PacketLogger.h>
#include <RakNetStatistics.h>
#include <BitStream.h>
#include "Streaming.h"
#include "Network/Players.h"
#include "v8world/Assembly.h"
#include "v8world/Mechanism.h"
#include "util/Profiling.h"
#include "util/Events.h"
#include <boost/scoped_ptr.hpp>

class RakPeer;
class RakPeerInterface;

class PacketLogger;

namespace RBX
{
	namespace Stats
	{
		class Item;
		class StatsService;
	}

	namespace Network
	{
		extern const char* sMarker;

		class Marker : public DescribedNonCreatable<Marker, Reflection::DescribedBase, &sMarker>, private boost::noncopyable
		{
		private:
			bool returned;
		public:
			static Reflection::SignalDesc<Marker, void(void)> event_Returned;
			static Reflection::SignalDesc<Marker, void(float)> event_Progress;

		private:
			Marker()
				: returned(false)
			{
			}
		public:
			long id() const;
			void fireReturned();

		public:
			static boost::shared_ptr<Marker> newMarker()
			{
				return boost::shared_ptr<Marker>(new Marker);
			}
			static Marker* fromId(long);
		};

		extern const char* sReplicator;

		class Replicator : public DescribedNonCreatable<Replicator, IdSerializer, &sReplicator>, public PluginInterface
		{
			class Item : private boost::noncopyable
			{
			public:
				enum ItemType
				{
					ItemTypeEnd,
					ItemTypeDelete,
					ItemTypeNew,
					ItemTypeChangeProperty,
					ItemTypeMarker
				};

			protected:
				Replicator& replicator;

			protected:
				Item(Replicator& replicator)
					: replicator(replicator)
				{
				}
			public:
				virtual ~Item();
				virtual void write(RakNet::BitStream& bitStream) = 0;
				
			public:
				static void writeItemType(RakNet::BitStream& stream, ItemType value);
				static void readItemType(RakNet::BitStream& stream, ItemType& value);
			};

			class ItemSender : private boost::noncopyable
			{
			private:
				Replicator& replicator;
				RakPeerInterface* peer;
				RakNet::BitStream bitStream;
				const int maxStreamSize;
			public:
				bool sentItems;

			private:
				void openPacket();
				void closePacket();
			public:
				ItemSender(Replicator& replicator, RakPeerInterface* peer);
				~ItemSender();
				bool send(Item& item);
			};

			class JobSender : public boost::noncopyable
			{
			private:
				const Assembly* firstA;
				const Mechanism* firstM;
				Replicator& replicator;
				RakPeerInterface* peer;
				bool hasOpenPacket;
				RakNet::BitStream bitStream;
				PacketPriority packetPriority;
			public:
				bool sentPacket;
			private:
				static const size_t maxStreamSize;

			private:
				void openPacket();
				void closePacket();
			public:
				JobSender(Replicator& replicator, RakPeerInterface* peer);
				void close();
				~JobSender();
				void setPacketPriority(PacketPriority packetPriority);
				bool report(const Mechanism& m);
				bool operator()(Mechanism&);
			};

			class MarkerItem : public Item
			{
			private:
				int id;

			public:
				MarkerItem(Replicator&, int);
				virtual void write(RakNet::BitStream& bitStream);
			};

			class NewInstanceItem : public Item
			{
			private:
				boost::shared_ptr<Instance> instance;

			public:
				NewInstanceItem(Replicator&, const boost::shared_ptr<Instance>&);
				virtual void write(RakNet::BitStream&);
			};

			class ReplicatorStatsItem : public Item // TODO (PR #124 has implementations of RBX::Stats classes)
			{
			private:
				Stats::Item* waitingRefs;
				Stats::Item* packetLoss;
				Stats::Item* ping;
				Stats::Item* instanceSize;
				Stats::Item* bps;
				const boost::shared_ptr<Replicator> replicator;
				const RakNetStatistics* statistics;
			public:
				size_t instanceCount;
				size_t instanceBits;

			public:
				ReplicatorStatsItem(const boost::shared_ptr<Replicator>&, const RakNetStatistics*);
				virtual void update();
			};

			class ChangePropertyItem : public Item
			{
			private:
				const Reflection::PropertyDescriptor& desc;
				const boost::shared_ptr<const Instance> instance;

			public:
				ChangePropertyItem(Replicator&, const boost::shared_ptr<const Instance>&, const Reflection::PropertyDescriptor&);
				virtual void write(RakNet::BitStream&);
			};

			class DeleteInstanceItem : public Item
			{
			private:
				const boost::shared_ptr<const Instance> instance;

			public:
				DeleteInstanceItem(Replicator& replicator, const boost::shared_ptr<const Instance>& instance);
				virtual void write(RakNet::BitStream&);
			};

		private:
			G3D::RealTime lastSendTime;
			MechanismTracker jobStagePos;
			G3D::RealTime lastCharacterSendTime;
			boost::shared_ptr<ReplicatorStatsItem> statsItem;
			std::map<const Reflection::PropertyDescriptor*, boost::shared_ptr<SharedStringDictionary>> strings;
			SharedStringDictionary classNames;
			SharedStringDictionary propNames;
		protected:
			std::vector<Instance*> replicationContainers;
			std::vector<boost::signals::connection> replicationContainerConnections;
			Players* players;
			std::list<boost::shared_ptr<Item>> pendingItems;
			std::set<const Instance*> pendingNewInstances;
			std::set<const Instance*> pendingDeleteInstances;
			bool sendPhysicsEnabled;
			const Reflection::Property* deserializeProperty;
			Instance* removingInstance;
			std::set<boost::weak_ptr<Instance>> deleteOnDisconnectInstances;
			std::map<boost::shared_ptr<Instance>, boost::signals::connection> propertyChangedConnections;
			std::queue<boost::shared_ptr<Marker>> incomingMarkers;
			std::map<const Name*, boost::shared_ptr<Instance>> defaultObjects;
			boost::scoped_ptr<Profiling::CodeProfiler> profileReplication;
			boost::scoped_ptr<Profiling::CodeProfiler> profileDataListening;
			boost::scoped_ptr<Profiling::CodeProfiler> profileDataIn;
			boost::scoped_ptr<Profiling::CodeProfiler> profileDataOut;
			boost::scoped_ptr<Profiling::CodeProfiler> profilePhysicsIn;
			boost::scoped_ptr<Profiling::CodeProfiler> profilePhysicsOut;
		private:
			bool receivedGlobals;
		public:
			bool disconnected;
			RakPeerInterface* peer;
			const SystemAddress remotePlayerId;

		public:
			static Reflection::BoundProp<int, 1> prop_maxDataModelSendBuffer;
			static Reflection::SignalDesc<Replicator, void(std::string, bool)> event_Disconnection;

		protected:
			virtual Player* findTargetPlayer();
			Mechanism* findTargetPlayerCharacterMechanism();
		public:
			Replicator(SystemAddress remotePlayerId, RakPeerInterface* peer);
			virtual ~Replicator();
			boost::shared_ptr<Instance> getPlayer()
			{
				return shared_from(findTargetPlayer());
			}
			virtual void Update(RakPeerInterface*);
			virtual PluginReceiveResult OnReceive(RakPeerInterface*, Packet*);
			virtual const Name& getClassName() const
			{
				return Name::getNullName();
			}
			virtual XmlElement* write();
			boost::shared_ptr<Reflection::DescribedBase> sendMarker();
			bool isSerializePending(const Instance* instance) const;
			bool isPropertyChangedPending(const Reflection::Property&) const;
			void requestCharacter();
			void closeConnection();
		protected:
			virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
			void onDescendentAdded(boost::shared_ptr<Instance>);
			void onDescendentRemoving(boost::shared_ptr<Instance>);
			void onPropertyChanged(boost::shared_ptr<Instance>, const Reflection::PropertyDescriptor*);
			const Instance* getDefault(const Name& className);
			void deleteDisconnectInstances();
			void connectPropertyChanged(boost::shared_ptr<Instance> instance);
			void disconnectPropertyChanged(boost::shared_ptr<Instance> instance);
			virtual bool wantReplicate(const Instance* source) const;
			virtual void onSentMarker(int);
		private:
			void disconnectAllPropertyChangedConnections();
			void connectToReplicationContainers();
			void removeFromPendingProperties(const Instance*);
			bool removeFromPendingNewInstances(const Instance*);
			bool removeFromPendingDeleteInstances(const Instance*);
			bool isChildOfPendingDeleteInstance(const Instance* instance) const;
			bool isInReplicationScope(const Instance* instance) const;
			bool sendItems();
			bool sendPhysicsPacket();
			void sendPhysicsData(RakNet::BitStream&, const Mechanism&);
			void sendPhysicsData(RakNet::BitStream&, const Assembly&);
			void createStatsItems(Stats::StatsService*);
			virtual bool remoteDeleteOnDisconnect(const Instance* instance) const;
			void serializeValue(const Reflection::ConstProperty&, bool, RakNet::BitStream&);
			void deserializeValue(RakNet::BitStream&, bool, Reflection::Property&);
			void skipValue(RakNet::BitStream&);
			void receiveData(Packet* packet);
			void readInstanceNew(RakNet::BitStream&);
			void readInstanceDelete(RakNet::BitStream&);
			void readChangedProperty(RakNet::BitStream&);
			void readMarker(RakNet::BitStream& inBitstream);
			bool getCameFromRemotePlayer(const Instance*) const;
			void setCameFromRemotePlayer(Instance*);
			SharedStringDictionary& getSharedDictionary(const Reflection::PropertyDescriptor& descriptor);
			PluginReceiveResult OnReceive_ID_TIMESTAMP(Packet*);
			virtual bool canSendItems();
		};

		extern const char* sPeer;

		class Peer : public Reflection::Described<Peer, &sPeer, Instance>, public PluginInterface, public Listener<RunService, Heartbeat>
		{
		private:
			boost::scoped_ptr<PacketLogger> logger;
			boost::scoped_ptr<Profiling::ThreadProfiler> profilePacketsThread;
		protected:
			const boost::scoped_ptr<RakPeer> rakPeer;

		protected:
			Peer();
			virtual ~Peer();
			virtual bool askAddChild(const Instance* instance) const
			{
				return fastDynamicCast<const Replicator>(instance) != NULL;
			}
			RakPeerInterface* peerInterface();
			void updateLogger();
			void updateNetworkSimulator();
			virtual void onEvent(const RunService* source, Heartbeat event);
			virtual void Update(RakPeerInterface* peer);
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		};
	}
}

class FilePacketLogger : public PacketLogger
{
private:
	FILE* packetLogFile;
	
public:
	FilePacketLogger(RBX::Network::Peer*);
	virtual ~FilePacketLogger();
	virtual void OnAttach(RakPeerInterface* peer);
	virtual void WriteLog(const char*);
};
