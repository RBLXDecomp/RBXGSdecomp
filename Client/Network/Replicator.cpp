#include <RakPeer.h>
#include <PacketLogger.h>
#include "Replicator.h"
#include "NetworkSettings.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/SimJobStage.h"
#include "util/Log.h"
#include "util/standardout.h"
#include <GetTime.h>
#include <g3d/system.h>

enum ValueType // NOTE: may not be intended for this file
{
	ValueType_nil,
	ValueType_string,
	ValueType_bool,
	ValueType_int,
	ValueType_float,
	ValueType_BrickColor,
	ValueType_Color3,
	ValueType_Vector3,
	ValueType_BrickVector,
	ValueType_CoordinateFrame,
	ValueType_Enum,
	ValueType_Ref,
	ValueType_ContentId
};

class ExtractorHack : public RakPeer
{
public:
	HANDLE getProcessPacketsThreadHandle() // the lengths you have to go through to get a protected member
	{
		return processPacketsThreadHandle;
	}
};

static void checkDisconnect(RBX::Instance* instance)
{
	RBX::Network::Replicator* repl = RBX::Instance::fastDynamicCast<RBX::Network::Replicator>(instance);

	if (repl && repl->disconnected)
	{
		repl->setParent(NULL);
	}
}

void writeValueType(bool includeLength, ValueType vt, RakNet::BitStream& outBitStream)
{
	if (includeLength)
	{
		unsigned char c = (unsigned char)vt;
		outBitStream.WriteBits(&c, 4, true);
	}
}

FilePacketLogger::FilePacketLogger(RBX::Network::Peer* peer)
	: packetLogFile(NULL)
{
	char filename[256];

	std::string pathName = RBX::Log::current()->logFile;
	pathName.erase(pathName.rfind('\\') + 1);

	sprintf(filename, "PacketLog%i.csv", RakNet::GetTime());
	pathName += filename;

	packetLogFile = fopen(pathName.c_str(), "wt");

	if (packetLogFile)
	{
		RBX::StandardOut::singleton()->print(RBX::MESSAGE_INFO, "Logging packets to %s", pathName.c_str());
	}
	else
	{
		RBX::StandardOut::singleton()->print(RBX::MESSAGE_WARNING, "Failed to create log file %s", pathName.c_str());
	}
}
	
void FilePacketLogger::OnAttach(RakPeerInterface* peer)
{
	PacketLogger::OnAttach(peer);
	LogHeader();
}

void FilePacketLogger::WriteLog(const char* str)
{
	if (packetLogFile)
		fputs(str, packetLogFile);
}

namespace RBX
{
	namespace Network
	{
		Peer::Peer()
			: profilePacketsThread(new Profiling::ThreadProfiler("Packets Thread")),
			  rakPeer(new RakPeer)
		{
			rakPeer->AttachPlugin(this);
		}

		Peer::~Peer() 
		{
			rakPeer->DetachPlugin(this);
		}

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

		void Peer::Update(RakPeerInterface* peer)
		{
			RBXASSERT(peer == rakPeer.get());

			profilePacketsThread->sample(static_cast<ExtractorHack*>(peer)->getProcessPacketsThreadHandle());
			PluginInterface::Update(peer); // TODO: blank function shared by multiple symbols. is this the correct one?
		}

		void Peer::updateLogger()
		{
			if (!NetworkSettings::singleton().logPackets)
			{
				logger.reset();
			}
			else if (!logger)
			{
				logger.reset(new FilePacketLogger(this));
				rakPeer->AttachPlugin(logger.get());
			}
		}

		void Peer::updateNetworkSimulator()
		{
			rakPeer->ApplyNetworkSimulator(
				NetworkSettings::singleton().maxSendBPS, 
				NetworkSettings::singleton().minExtraPing,
				NetworkSettings::singleton().extraPingVariance
			);
		}

		Replicator::Replicator(SystemAddress remotePlayerId, RakPeerInterface* peer)
			: lastSendTime(G3D::System::getLocalTime()),
			  lastCharacterSendTime(G3D::System::getLocalTime()),
			  profileReplication(new Profiling::CodeProfiler("Replication")),
			  profileDataListening(new Profiling::CodeProfiler("Data Listening")),
			  profileDataIn(new Profiling::CodeProfiler("Data In")),
			  profileDataOut(new Profiling::CodeProfiler("Data Out")),
			  profilePhysicsIn(new Profiling::CodeProfiler("Physics In")),
			  profilePhysicsOut(new Profiling::CodeProfiler("Physics Out")),
			  remotePlayerId(remotePlayerId),
			  peer(peer),
			  disconnected(false),
			  receivedGlobals(false),
			  sendPhysicsEnabled(false),
			  players(NULL),
			  deserializeProperty(NULL),
			  removingInstance(NULL)
		{
			peer->AttachPlugin(this);
			setName(remotePlayerId.ToString(true));

			profileDataListening->parent = profileReplication.get();
			profileDataIn->parent = profileReplication.get();
			profileDataOut->parent = profileReplication.get();
			profilePhysicsIn->parent = profileReplication.get();
			profilePhysicsOut->parent = profileReplication.get();
		}

		Replicator::~Replicator()
		{
			if (peer)
				peer->DetachPlugin(this);
		}

		bool Replicator::wantReplicate(const Instance* source) const
		{
			return fastDynamicCast<const Camera>(source) == NULL ? true : false;
		}

		bool Replicator::canSendItems()
		{
			return receivedGlobals;
		}

		void Replicator::closeConnection()
		{
			peer->CloseConnection(remotePlayerId, true);
			setParent(NULL);
		}

		Player* Replicator::findTargetPlayer()
		{
			return players ? players->getLocalPlayer() : NULL;
		}

		Mechanism* Replicator::findTargetPlayerCharacterMechanism()
		{
			Player* player = findTargetPlayer();

			if (player && player->getCharacter())
			{
				PartInstance* part = player->getCharacter()->getPrimaryPart();

				if (part)
					return Mechanism::getMechanismFromPrimitive(part->getPrimitive());
			}
			
			return NULL;
		}

		bool Replicator::isSerializePending(const Instance* instance) const
		{
			return pendingNewInstances.find(instance) != pendingNewInstances.end();
		}

		void Replicator::disconnectAllPropertyChangedConnections()
		{
			while (propertyChangedConnections.size() > 0)
			{
				propertyChangedConnections.begin()->second.disconnect();
				propertyChangedConnections.erase(propertyChangedConnections.begin());
			}
		}

		void Replicator::requestCharacter()
		{
			RakNet::BitStream bitStream;

			bitStream << 'L';

			Player* player = findTargetPlayer();
			if (!player)
				throw std::runtime_error("Attempting to send a Character request without a local Player");

			serializeId(bitStream, player);

			if (NetworkSettings::singleton().printInstances)
			{
				Guid::Data id;
				player->getGuid().extract(id);

				StandardOut::singleton()->print(MESSAGE_INFO, "Replicator: Requesting character for %s", id.readableString(4).c_str());
			}

			peer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, remotePlayerId, false);
		}

		void Replicator::disconnectPropertyChanged(boost::shared_ptr<Instance> instance)
		{
			std::map<boost::shared_ptr<Instance>, boost::signals::connection>::iterator iter = propertyChangedConnections.find(instance);

			if (iter != propertyChangedConnections.end())
			{
				iter->second.disconnect();
				propertyChangedConnections.erase(iter);
			}
		}

		void Replicator::deleteDisconnectInstances()
		{
			std::set<boost::weak_ptr<Instance>>::iterator iter = deleteOnDisconnectInstances.begin();
			std::set<boost::weak_ptr<Instance>>::iterator end = deleteOnDisconnectInstances.end();

			for (; iter != end; iter++)
			{
				boost::shared_ptr<Instance> instance = iter->lock();
				if (instance)
					instance->setParent(NULL);
			}

			deleteOnDisconnectInstances.clear();
		}

		bool Replicator::isInReplicationScope(const Instance* instance) const
		{
			std::vector<Instance*>::const_iterator begin = replicationContainers.begin();
			std::vector<Instance*>::const_iterator end = replicationContainers.end();

			if (std::find_if(begin, end, boost::bind(&Instance::isDescendentOf, instance, _1)) != end)
			{
				return true;
			}
			else
			{
				return std::find(begin, end, instance) != end ? true : false;
			}
		}

		void Replicator::readMarker(RakNet::BitStream& inBitstream)
		{
			intptr_t marker;
			inBitstream >> marker;

			if (NetworkSettings::singleton().printInstances)
			{
				StandardOut::singleton()->print(MESSAGE_INFO, "Received marker %d from %s", (intptr_t)marker, remotePlayerId.ToString());
			}

			RBXASSERT(reinterpret_cast<Marker*>(marker) == incomingMarkers.front().get());

			Marker::event_Returned.fire(incomingMarkers.front().get());
			incomingMarkers.pop();
		}

		boost::shared_ptr<Reflection::DescribedBase> Replicator::sendMarker()
		{
			boost::shared_ptr<Marker> marker = Marker::newMarker();

			RakNet::BitStream bitStream;
			bitStream << 'N';
			bitStream << (intptr_t)marker.get();

			if (NetworkSettings::singleton().printInstances)
			{
				StandardOut::singleton()->print(MESSAGE_INFO, "Replicator: Requesting Marker %d of %s", (intptr_t)marker.get(), remotePlayerId.ToString());
			}

			incomingMarkers.push(marker);
			peer->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, remotePlayerId, false);

			return marker;
		}

		bool Replicator::sendPhysicsPacket()
		{
			if (!sendPhysicsEnabled)
			{
				return false;
			}
			else if (!peer->GetStatistics(remotePlayerId)) // TODO: vtable offset does not match due to missing function in RakPeerInterface
			{
				return false;
			}
			else
			{
				Profiling::Mark mark(*profilePhysicsOut, false);
				JobSender jobSender(*this, peer);

				Workspace* workspace = ServiceProvider::find<Workspace>(this);
				if (workspace)
				{
					SimJobStage& sim = workspace->getWorld()->getSimJobStage();
					Mechanism* mech = findTargetPlayerCharacterMechanism();
					if (mech)
					{
						G3D::RealTime t = G3D::System::getLocalTime();
						if (t > lastCharacterSendTime + 0.1)
						{
							jobSender.setPacketPriority(MEDIUM_PRIORITY);
							jobSender.report(*mech);
							jobSender.setPacketPriority(MEDIUM_PRIORITY);

							lastCharacterSendTime = t;
						}
						else
						{
							mech = NULL;
						}
					}

					sim.reportMechanisms(jobSender, jobStagePos, mech);
				}

				return jobSender.sentPacket;
			}
		}

		bool Replicator::sendItems()
		{
			if (!canSendItems())
			{
				return false;
			}
			else
			{
				Profiling::Mark mark(*profileDataOut, false);
				ItemSender sender(*this, peer);

				while (pendingItems.size() > 0)
				{
					boost::shared_ptr<Item> item = *pendingItems.begin();

					if (item)
					{
						if (!sender.send(*item))
							break;
					}

					pendingItems.pop_front();
				}

				return sender.sentItems;
			}
		}

		const Instance* Replicator::getDefault(const Name& className)
		{
			Security::Impersonator impersonate(Security::Replicator);

			std::map<const Name*, boost::shared_ptr<Instance>>::iterator iter = defaultObjects.find(&className);

			if (iter == defaultObjects.end())
			{
				boost::shared_ptr<Instance> instance = AbstractFactoryProduct::create(className);

				if (!instance)
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Replication: Can\'t create default object of type %s", className.c_str());
				}

				defaultObjects[&className] = instance;
				return instance.get();
			}
			else
			{
				return iter->second.get();
			}
		}

		SharedStringDictionary& Replicator::getSharedDictionary(const Reflection::PropertyDescriptor& descriptor)
		{
			std::map<const Reflection::PropertyDescriptor*, boost::shared_ptr<SharedStringDictionary>>::iterator iter = strings.find(&descriptor);

			if (iter == strings.end())
			{
				boost::shared_ptr<SharedStringDictionary> result(new SharedStringDictionary);

				strings[&descriptor] = result;
				return *result;
			}
			else
			{
				return *iter->second;
			}
		}

		void Replicator::receiveData(Packet* packet)
		{
			Profiling::Mark mark(*profileDataIn, false);

			RakNet::BitStream inBitstream(packet->data, packet->length, false);
			inBitstream.IgnoreBits(8);

			Security::Impersonator impersonate(Security::Replicator);

			while (true)
			{
				Item::ItemType itemType;
				Item::readItemType(inBitstream, itemType);
				
				switch (itemType)
				{
				case Item::ItemTypeDelete:
					readInstanceDelete(inBitstream);
					break;
				case Item::ItemTypeNew:
					readInstanceNew(inBitstream);
					break;
				case Item::ItemTypeChangeProperty:
					readChangedProperty(inBitstream);
					break;
				case Item::ItemTypeMarker:
					readMarker(inBitstream);
					break;
				case Item::ItemTypeEnd:
					return;
				}
			}
		}

		void Replicator::Item::readItemType(RakNet::BitStream& stream, ItemType& value)
		{
			value = ItemTypeEnd;

			stream.ReadBits((unsigned char*)&value, 2, true);

			if (value < 1 || value > 3) // neither of Delete, New, ChangeProperty
			{
				stream.Read(value);
			}
		}

		void Replicator::Item::writeItemType(RakNet::BitStream& stream, ItemType value)
		{
			if (value >= 1 && value <= 3) // Delete, New, ChangeProperty
			{
				stream.WriteBits((unsigned char*)&value, 2, true);
			}
			else
			{
				unsigned char zeroes = 0;
				stream.WriteBits(&zeroes, 2, true);
				stream.Write(value);
			}
		}

		Replicator::ItemSender::ItemSender(Replicator& replicator, RakPeerInterface* peer)
			: replicator(replicator),
			  peer(peer),
			  maxStreamSize(G3D::iRound(NetworkSettings::singleton().dataPacketSize * (peer->GetMTUSize(replicator.remotePlayerId) - 128))),
			  sentItems(false)
		{
		}

		Replicator::ItemSender::~ItemSender()
		{
			if (bitStream.GetNumberOfBitsUsed() > 0)
			{
				Item::writeItemType(bitStream, Item::ItemTypeEnd);
				peer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, replicator.remotePlayerId, false);
				bitStream.Reset();
			}
		}

		bool Replicator::ItemSender::send(Item& item)
		{
			if (bitStream.GetNumberOfBytesUsed() < maxStreamSize)
			{
				if (bitStream.GetNumberOfBitsUsed() == 0)
				{
					bitStream << 'M';
				}

				item.write(bitStream);
				sentItems = true;

				return true;
			}
			else
			{
				return false;
			}
		}

		Replicator::JobSender::JobSender(Replicator& replicator, RakPeerInterface* peer)
			: replicator(replicator),
			  peer(peer),
			  firstA(NULL),
			  firstM(NULL),
			  packetPriority(MEDIUM_PRIORITY),
			  sentPacket(false),
			  hasOpenPacket(false),
			  bitStream(1364) // what
		{
		}

		Replicator::JobSender::~JobSender()
		{
			if (hasOpenPacket)
			{
				closePacket();
			}
		}

		void Replicator::JobSender::setPacketPriority(PacketPriority packetPriority)
		{
			if (this->packetPriority != packetPriority)
			{
				closePacket();
				this->packetPriority = packetPriority;
			}
		}

		void Replicator::JobSender::closePacket()
		{
			replicator.serializeId(bitStream, NULL);
			peer->Send(&bitStream, packetPriority, UNRELIABLE, 0, replicator.remotePlayerId, false);
			hasOpenPacket = false;
		}

		bool Replicator::JobSender::report(const Mechanism& m)
		{
			if (!hasOpenPacket)
			{
				bitStream.Reset();

				bitStream << (char)0x18;
				bitStream << RakNet::GetTime();
				bitStream << 'O';

				hasOpenPacket = true;
				sentPacket = true;
			}

			replicator.sendPhysicsData(bitStream, m);
			replicator.serializeId(bitStream, NULL);

			return bitStream.GetNumberOfBytesUsed() < 1364u;
		}

		void Replicator::MarkerItem::write(RakNet::BitStream& bitStream)
		{
			writeItemType(bitStream, ItemTypeMarker);
			bitStream << (int)id;

			if (NetworkSettings::singleton().printInstances)
			{
				StandardOut::singleton()->print(MESSAGE_INFO, "Replication: Sending marker %d to %s", id, replicator.remotePlayerId.ToString());
			}

			replicator.onSentMarker(id);
		}
	}
}
