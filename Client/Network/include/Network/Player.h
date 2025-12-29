#pragma once
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"

namespace RBX
{
	class ServiceProvider;
	class ModelInstance;
	class Backpack;

	namespace Network
	{
		struct CharacterAdded
		{
		public:
			const boost::shared_ptr<Instance> character;
		  
		public:
			//CharacterAdded(const CharacterAdded&);
			CharacterAdded(Instance*);
			~CharacterAdded();
		};

		struct CharacterRemoving
		{
		public:
			const boost::shared_ptr<Instance> character;
		  
		public:
			//CharacterRemoving(const CharacterRemoving&);
			CharacterRemoving(Instance*);
			~CharacterRemoving();
		};

		extern const char* sPlayer;
		class Player : public DescribedCreatable<Player, Instance, &sPlayer>,
					   public Notifier<Player, CharacterAdded>,
					   public Notifier<Player, CharacterRemoving>
		{
		private:
			boost::shared_ptr<ModelInstance> character;
			BrickColor teamColor;
			bool neutral;
			boost::signals::scoped_connection characterDiedConnection;
			std::string characterAppearance;
			bool under13;
			bool superSafeChat;
			int userId;
			double lastActivityTime;

		public:
			static Reflection::BoundProp<int, 1> prop_userId;
			static Reflection::BoundProp<bool, 1> prop_Under13;
			static Reflection::BoundProp<bool, 1> prop_SuperSafeChat;
			static Reflection::SignalDesc<Player, void(std::string, boost::shared_ptr<Instance>)> event_Chatted;

		private:
			virtual bool askAddChild(const Instance*) const;
			virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
			void onCharacterChangedFrontend();
			void registerLocalPlayerNotIdle();
		public:
			//Player(const Player&);
			Player();
			virtual ~Player();
		public:
			virtual XmlElement* write();
			virtual void setName(const std::string&);
			ModelInstance* getCharacter() const;
			void setCharacter(ModelInstance*);
			BrickColor getTeamColor() const;
			void setTeamColor(BrickColor);
			bool getNeutral() const;
			void setNeutral(bool);
			std::string getCharacterAppearance() const;
			void setCharacterAppearance(const std::string&);
			bool getUnder13() const;
			bool getSuperSafeChat() const;
			void setUnder13(bool);
			void setSuperSafeChat(bool);
			int getUserID() const;
			void rebuildBackpack();
			Backpack* getPlayerBackpack() const;
			void loadCharacter();
			void removeCharacter();
			void removeCharacterAppearance();
			void loadCharacterAppearance();
		private:
			void onCharacterDied();
			void doPeriodicIdleCheck();
		public:
			//Player& operator=(const Player&);

		public:
			static void onLocalPlayerNotIdle(ServiceProvider*);
		};
	}
}
