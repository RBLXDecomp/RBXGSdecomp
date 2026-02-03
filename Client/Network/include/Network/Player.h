#pragma once
#include <winsock2.h>
#include "Network/Players.h"
#include "security/SecurityContext.h"
#include "humanoid/Humanoid.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "v8datamodel/BrickColor.h"
#include "v8datamodel/Backpack.h"
#include "v8datamodel/Workspace.h"

namespace RBX
{
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
			G3D::RealTime lastActivityTime;

		public:
			static Reflection::BoundProp<int, 1> prop_userId;
			static Reflection::BoundProp<bool, 1> prop_Under13;
			static Reflection::BoundProp<bool, 1> prop_SuperSafeChat;
			static Reflection::SignalDesc<Player, void(std::string, boost::shared_ptr<Instance>)> event_Chatted;

		private:
			virtual bool askAddChild(const Instance*) const;
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
			void onCharacterChangedFrontend();
			void registerLocalPlayerNotIdle();
		public:
			//Player(const Player&);
			Player();
			virtual ~Player();
		public:
			virtual XmlElement* write();
			virtual void setName(const std::string& value);
			ModelInstance* getCharacter() const
			{
				return character.get();
			}
			void setCharacter(ModelInstance*);
			BrickColor getTeamColor() const
			{
				return teamColor;
			}
			void setTeamColor(BrickColor value);
			bool getNeutral() const
			{
				return neutral;
			}
			void setNeutral(bool value);
			std::string getCharacterAppearance() const
			{
				return characterAppearance;
			}
			void setCharacterAppearance(const std::string& value);
			bool getUnder13() const;
			bool getSuperSafeChat() const;
			void setUnder13(bool value)
			{
				prop_Under13.setValue(this, value);
			}
			void setSuperSafeChat(bool value)
			{
				prop_SuperSafeChat.setValue(this, value);
			}
			int getUserID() const
			{
				return userId;
			}
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
