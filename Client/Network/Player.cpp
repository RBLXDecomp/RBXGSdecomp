#include "Client.h"
#include "Player.h"
#include "security/SecurityContext.h"
#include "v8datamodel/TimerService.h"

RBX::Reflection::PropDescriptor<RBX::Network::Player, RBX::BrickColor> prop_teamColor("TeamColor", "Team", &RBX::Network::Player::getTeamColor, &RBX::Network::Player::setTeamColor, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::Network::Player, bool> prop_neutral("Neutral", "Team", &RBX::Network::Player::getNeutral, &RBX::Network::Player::setNeutral, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::Network::Player, std::string> prop_characterAppearance("CharacterAppearance", "Data", &RBX::Network::Player::getCharacterAppearance, &RBX::Network::Player::setCharacterAppearance, RBX::Reflection::PropertyDescriptor::STANDARD);

RBX::Reflection::RefPropDescriptor<RBX::Network::Player, RBX::ModelInstance> prop_Character("Character", "Data", &RBX::Network::Player::getCharacter, &RBX::Network::Player::setCharacter, RBX::Reflection::PropertyDescriptor::STANDARD);

RBX::Reflection::SignalDesc<RBX::Network::Player, void(float)> event_Idled("Idled", "time");

static void addChild(const boost::shared_ptr<RBX::ModelInstance>& parent, const boost::shared_ptr<RBX::Instance>& child)
{
	child->setParent(parent.get());
}

namespace RBX
{
	namespace Network
	{
		Player::Player()
			: teamColor(BrickColor::lego_1),
			  neutral(true),
			  under13(false),
			  superSafeChat(false),
			  userId(0),
			  lastActivityTime(0.0)
		{
			Security::Context::current().requirePermission(Security::Administrator, "create a Player");
			setName("Player");
		}

		Player::~Player()
		{
			setCharacter(NULL);
		}

		Backpack* Player::getPlayerBackpack() const
		{
			Backpack* backpack = findFirstChildOfType<Backpack>();

			RBXASSERT(backpack != NULL);
			return backpack;
		}

		void Player::onCharacterChangedFrontend()
		{
			RBXASSERT(Players::frontendProcessing(this, true));

			Player* localPlayer = Players::findLocalPlayer(this);
			if (this == localPlayer)
			{
				Workspace* workspace = ServiceProvider::find<Workspace>(this);
				RBXASSERT(workspace != NULL);

				if (!character)
				{
					workspace->getCamera()->setCameraType(Camera::FIXED_CAMERA);
					workspace->getCamera()->setDistanceFromTarget(0.0f);
					workspace->getCamera()->zoom(-1.0f);
				}
				else
				{
					workspace->getCamera()->setCameraSubject(Humanoid::modelIsCharacter(character.get()));
					workspace->getCamera()->setCameraType(Camera::CUSTOM_CAMERA);
					workspace->getCamera()->setDistanceFromTarget(13.0f);
					workspace->getCamera()->zoom(-1.0f);
					workspace->setDefaultMouseCommand();
				}
			}
		}

		void Player::setTeamColor(BrickColor value)
		{
			if (value != teamColor)
			{
				teamColor = value;
				raisePropertyChanged(prop_teamColor);
			}
		}

		void Player::setNeutral(bool value)
		{
			if (value != neutral)
			{
				neutral = value;
				raisePropertyChanged(prop_neutral);
			}
		}

		void Player::removeCharacter()
		{
			if (!Players::backendProcessing(this, true))
				throw std::runtime_error("RemoveCharacter can only be called by the backend server");

			setCharacter(NULL);
		}

		void Player::setName(const std::string& value)
		{
			Security::Context::current().requirePermission(Security::Administrator, "set a Player's name");
			Instance::setName(value);
		}

		void Player::setCharacterAppearance(const std::string& value)
		{
			if (value != characterAppearance)
			{
				characterAppearance = value;
				if (Players::backendProcessing(this, false))
					loadCharacterAppearance();

				raisePropertyChanged(prop_characterAppearance);
			}
		}

		void Player::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
		{
			if (oldProvider && Players::backendProcessing(oldProvider, true))
				setCharacter(NULL);

			Instance::onServiceProvider(oldProvider, newProvider);

			if (newProvider && Players::frontendProcessing(newProvider, true))
				onCharacterChangedFrontend();

			if (!oldProvider && Players::frontendProcessing(newProvider, true))
			{
				RBXASSERT(Players::frontendProcessing(this, true));
				lastActivityTime = G3D::System::getLocalTime();
				doPeriodicIdleCheck();
			}
		}

		void Player::doPeriodicIdleCheck()
		{
			if (ServiceProvider::findServiceProvider(this))
			{
				RBXASSERT(Players::frontendProcessing(this, true));
				if (lastActivityTime != 0.0)
				{
					G3D::RealTime idleTime = G3D::System::getLocalTime() - lastActivityTime;
					if (idleTime > 120.0)
					{
						Players* players = ServiceProvider::find<Players>(this);
						if (players && players->getLocalPlayer() && Players::clientIsPresent(this, true))
						{
							event_Idled.fire(this, idleTime);
						}
					}
				}

				TimerService* timerService = ServiceProvider::create<TimerService>(this);

				if (timerService)
					timerService->delay(boost::bind(&Player::doPeriodicIdleCheck, shared_from(this)), 30.0);
			}
		}

		void Player::setCharacter(ModelInstance* value)
		{
			if (value != character.get())
			{
				if (character.get())
				{
					characterDiedConnection.disconnect();

					Notifier<Player, CharacterRemoving>::raise(character.get());

					if (Players::backendProcessing(this, false))
						character->setParent(NULL);

					character.reset();
				}

				if (value)
				{
					character = shared_from(value);

					Notifier<Player, CharacterAdded>::raise(character.get());

					if (Players::frontendProcessing(this, false))
						onCharacterChangedFrontend();
				}

				raisePropertyChanged(prop_Character);
			}
		}
	}
}
