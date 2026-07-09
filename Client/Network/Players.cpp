#include <RakPeer.h>
#include <BitStream.h>
#include "Client.h"
#include "Players.h"
#include "Streaming.h"

template<class Class>
class PluginInterfaceAdapter : public PluginInterface
{
private:
	Class* c;
protected:
	PluginInterfaceAdapter(Class* c)
		: PluginInterface(),
		  c(c)
	{
	}
public:
	virtual PluginReceiveResult OnReceive(RakPeerInterface* peer, Packet* packet);
};

static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, void(boost::shared_ptr<RBX::Instance>, std::string), 2> funcReportAbuse(&RBX::Network::Players::reportAbuse, "ReportAbuse", "player", "message", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<const std::vector<boost::shared_ptr<RBX::Instance>>>(void), 0> func_players(&RBX::Network::Players::getPlayers, "players", RBX::Reflection::FunctionDescriptor::AnyCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<const std::vector<boost::shared_ptr<RBX::Instance>>>(void), 0> func_GetPlayers(&RBX::Network::Players::getPlayers, "GetPlayers", RBX::Reflection::FunctionDescriptor::AnyCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<RBX::Instance>(int), 1> func_createLocalPlayer(&RBX::Network::Players::createLocalPlayer, "CreateLocalPlayer", "userId", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, void(std::string), 1> func_setAbuseReportUrl(&RBX::Network::Players::setAbuseReportUrl, "SetAbuseReportUrl", "url", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<RBX::Instance>(boost::shared_ptr<RBX::Instance>), 1> func_playerFromCharacterOld(&RBX::Network::Players::playerFromCharacter, "playerFromCharacter", "character", RBX::Reflection::FunctionDescriptor::AnyCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<RBX::Instance>(boost::shared_ptr<RBX::Instance>), 1> func_playerFromCharacter(&RBX::Network::Players::playerFromCharacter, "GetPlayerFromCharacter", "character", RBX::Reflection::FunctionDescriptor::AnyCaller);

static RBX::Reflection::SignalDesc<RBX::Network::Players, void(boost::shared_ptr<RBX::Instance>)> event_PlayerAdded("PlayerAdded", "player");
static RBX::Reflection::SignalDesc<RBX::Network::Players, void(boost::shared_ptr<RBX::Instance>)> event_PlayerRemoving("PlayerRemoving", "player");

static RBX::Reflection::PropDescriptor<RBX::Network::Players, int> propPlayerCount("NumPlayers", "Data", &RBX::Network::Players::numPlayers, NULL, RBX::Reflection::PropertyDescriptor::UI);
static RBX::Reflection::PropDescriptor<RBX::Network::Players, int> propPlayerMaxCount("MaxPlayers", "Data", &RBX::Network::Players::getMaxPlayers, &RBX::Network::Players::setMaxPlayers, RBX::Reflection::PropertyDescriptor::STANDARD);

static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, boost::shared_ptr<RBX::Instance>(int), 1> func_GetPlayerByID(&RBX::Network::Players::getPlayerByID, "GetPlayerByID", "userID", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Players, void(std::string), 1> funcChat(&RBX::Network::Players::chat, "Chat", "message", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

namespace RBX
{
	namespace Network
	{
		class Players::Plugin : public PluginInterfaceAdapter<Players>
		{
		public:
			Plugin(Players* players)
				: PluginInterfaceAdapter<Players>(players)
			{
			}
		};

		Players::~Players()
		{
			if (peer)
				peer->DetachPlugin(plugin.get());

			peer = NULL;
		}

		bool Players::clientIsPresent(const Instance* context, bool testInDatamodel)
		{
			return Client::clientIsPresent(context, testInDatamodel);
		}

		bool Players::askAddChild(const Instance* instance) const
		{
			return fastDynamicCast<const Player>(instance) != NULL;
		}

		void Players::setConnection(RakPeerInterface* peer)
		{
			if (this->peer)
				this->peer->DetachPlugin(plugin.get());

			this->peer = peer;

			if (peer)
				peer->AttachPlugin(plugin.get());
		}

		Player* Players::findLocalPlayer(const Instance* context)
		{
			Players* players = ServiceProvider::find<Players>(context);
			return players ? players->getLocalPlayer() : NULL;
		}

		ModelInstance* Players::findLocalCharacter(const Instance* context)
		{
			Player* player = Players::findLocalPlayer(context);
			return player ? player->getCharacter() : NULL;
		}

		void Players::setAbuseReportUrl(std::string value)
		{
			if (value.empty())
				abuseReporter.reset(NULL);
			else
				abuseReporter.reset(new AbuseReporter(value));
		}

		void Players::setMaxPlayers(int value)
		{
			if (value != maxPlayers)
			{
				maxPlayers = value;
				raisePropertyChanged(propPlayerMaxCount);
			}
		}

		void Players::reportAbuse(boost::shared_ptr<Instance> player, std::string comment)
		{
			reportAbuse(fastDynamicCast<Player>(player.get()), comment);
		}

		Player* Players::getPlayerFromCharacter(Instance* character)
		{
			Players* players = ServiceProvider::find<Players>(character);
			if (players)
			{
				boost::shared_ptr<Instance> found = players->playerFromCharacter(shared_from(character));
				return static_cast<Player*>(found.get());
			}
			else
			{
				return NULL;
			}
		}

		void Players::chat(std::string message)
		{
			if (!localPlayer)
				throw std::runtime_error("No local Player to chat from");

			RakNet::BitStream bitStream;
			bitStream << 'P';

			Guid::Data data;
			localPlayer->getGuid().extract(data);

			bitStream << data.scope->name;
			bitStream << data.index;
			bitStream << message;

			peer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE, 2, UNASSIGNED_SYSTEM_ADDRESS, true);

			ChatMessage event = {message, localPlayer, boost::shared_ptr<Player>()};
			Notifier<Players, ChatMessage>::raise(event);
		}

		bool Players::backendProcessing(const Instance* context, bool testInDatamodel)
		{
			const ServiceProvider* sp = ServiceProvider::findServiceProvider(context);
			RBXASSERT(!testInDatamodel || sp);

			return sp && !Client::clientIsPresent(context, testInDatamodel);
		}

		boost::shared_ptr<Instance> Players::playerFromCharacter(boost::shared_ptr<Instance> character)
		{
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> myPlayers = getPlayers();

			std::vector<boost::shared_ptr<Instance>>::const_iterator iter = myPlayers->begin();
			std::vector<boost::shared_ptr<Instance>>::const_iterator end = myPlayers->end();

			for (; iter != end; iter++)
			{
				if (static_cast<Player*>(iter->get())->getCharacter() == static_cast<ModelInstance*>(character.get()))
					return *iter;
			}

			return boost::shared_ptr<Instance>();
		}

		boost::shared_ptr<Instance> Players::getPlayerByID(int userID)
		{
			std::vector<boost::shared_ptr<Instance>>::const_iterator iter = players->begin();
			std::vector<boost::shared_ptr<Instance>>::const_iterator end = players->end();

			for (; iter != end; iter++)
			{
				if (static_cast<Player*>(iter->get())->getUserID() == userID)
					return *iter;
			}

			return boost::shared_ptr<Instance>();
		}

		void AbuseReport::addMessage(const ChatMessage& cm)
		{
			int userId = cm.source ? Player::prop_userId.getValue(cm.source.get()) : 0;
			AbuseReport::Message m = {userId, cm.message};
			messages.push_back(m);
		}

		AbuseReporter::AbuseReporter(std::string abuseUrl)
			: _data(new AbuseReporter::data)
		{
			requestProcessor.reset(new worker_thread(boost::bind(&AbuseReporter::processRequests, _data, abuseUrl), "rbx_abusereporter"));
		}

		void AbuseReporter::add(AbuseReport& r, const std::list<ChatMessage>& chatHistory)
		{
			std::for_each(chatHistory.begin(), chatHistory.end(), boost::bind(&AbuseReport::addMessage, &r, _1));

			{
				boost::mutex::scoped_lock lock(_data->requestSync);
				_data->queue.push(r);
			}

			requestProcessor->wake();
		}
	}
}
