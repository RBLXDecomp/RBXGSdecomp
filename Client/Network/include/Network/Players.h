#pragma once
#include "Player.h"
#include "SuperSafeChanged.h"
#include "v8tree/Service.h"
#include "v8datamodel/ModelInstance.h"
#include "Client.h"
#include <list>
#include <queue>

class RakPeerInterface;
struct Packet;

template<class Class>
class PluginInterfaceAdapter : public PluginInterface
{
private:
	Class* c;
protected:
	PluginInterfaceAdapter(Class*);
public:
	virtual PluginReceiveResult OnReceive(RakPeerInterface* peer, Packet* packet);
};

namespace RBX
{
	namespace Network
	{
		class Player;
		struct CharacterAdded;

		struct ChatMessage
		{
		public:
			const std::string message;
			const boost::shared_ptr<Player> source;
			const boost::shared_ptr<Player> destination;
		  
		public:
			~ChatMessage();
			//ChatMessage(const ChatMessage&);
		};

		struct AbuseReport
		{
		public:
			struct Message
			{
			public:
				int userID;
				std::string text;
			  
			public:
				//Message(const Message&);
				Message();
				~Message();
			public:
				//Message& operator=(const Message&);
			};

		public:
			int submitterID;
			int allegedAbuserID;
			std::string comment;
			std::list<Message> messages;

		public:
			void addMessage(const ChatMessage&);
		public:
			//AbuseReport(const AbuseReport&);
			AbuseReport();
			~AbuseReport();
		public:
			//AbuseReport& operator=(const AbuseReport&);
		};

		class AbuseReporter
		{
		private:
			struct data
			{
			public:
				std::queue<AbuseReport> queue;
				boost::mutex requestSync;
			  
			public:
				//data(const data&);
				data();
				~data();
			public:
				//data& operator=(const data&);
			};

		private:
			boost::shared_ptr<data> _data;
			boost::scoped_ptr<worker_thread> requestProcessor;
		  
		public:
			//AbuseReporter(const AbuseReporter&);
			AbuseReporter(std::string);
		public:
			void add(AbuseReport&, const std::list<ChatMessage>&);
		public:
			~AbuseReporter();
		public:
			//AbuseReporter& operator=(const AbuseReporter&);

		private:
			static worker_thread::work_result processRequests(boost::shared_ptr<data>, std::string);
		};

		extern const char* sPlayers;
		class Players : public DescribedNonCreatable<Players, Instance, &sPlayers>,
						public Service,
						public Notifier<Players, ChatMessage>,
						public Notifier<Players, SuperSafeChanged>,
						public Listener<Player, CharacterAdded>
		{
		private:
			class Plugin : public PluginInterfaceAdapter<Players>
			{
				Plugin(Players*);
			};

		private:
			boost::scoped_ptr<AbuseReporter> abuseReporter;
			std::list<ChatMessage> chatHistory;
			boost::scoped_ptr<Plugin> plugin;
			CopyOnWrite<std::vector<boost::shared_ptr<Instance>>> players;
			boost::shared_ptr<Player> localPlayer;
			RakPeerInterface* peer;
			int maxPlayers;

		public:
			static Reflection::RefPropDescriptor<Players, Player> propLocalPlayer;
		  
		private:
			virtual void onEvent(const Player*, CharacterAdded);
			virtual void onChildChanged(Instance*, const PropertyChanged&);
		public:
			//Players(const Players&);
			Players();
			virtual ~Players();
		public:
			bool superSafeOn() const;
			boost::shared_ptr<Instance> createLocalPlayer(int);
			Player* getLocalPlayer() const
			{
				return localPlayer.get();
			}
			int getNumPlayers() const;
			int numPlayers() const
			{
				return (int)players->size();
			}
			int getMaxPlayers() const
			{
				return maxPlayers;
			}
			void setMaxPlayers(int);
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> getPlayers()
			{
				return players.read();
			}
			void chat(std::string);
			void reportAbuse(boost::shared_ptr<Instance>, std::string);
			void reportAbuse(Player*, std::string);
			std::list<ChatMessage>::const_iterator chatHistory_begin();
			std::list<ChatMessage>::const_iterator chatHistory_end();
			bool canReportAbuse() const;
			void setAbuseReportUrl(std::string);
			bool OnReceive(RakPeerInterface*, Packet*);
			void setConnection(RakPeerInterface* peer);
			boost::shared_ptr<Instance> playerFromCharacter(boost::shared_ptr<Instance>);
			boost::shared_ptr<Instance> getPlayerByID(int);
		protected:
			virtual bool askAddChild(const Instance* instance) const;
			virtual void onChildAdded(Instance*);
			virtual void onChildRemoving(Instance*);
		private:
			void addChatMessage(const ChatMessage&);
		public:
			//Players& operator=(const Players&);
		  
		public:
			static Player* getPlayerFromCharacter(Instance*);
			static ModelInstance* findLocalCharacter(const Instance*);
			static Player* findLocalPlayer(const Instance*);
			static bool clientIsPresent(const Instance* context, bool testInDatamodel);
			static bool serverIsPresent(const Instance*, bool);
			static bool frontendProcessing(const Instance*, bool);
			static bool backendProcessing(const Instance*, bool);
		};
	}
}
