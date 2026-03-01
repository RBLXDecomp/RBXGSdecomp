#include "Player.h"
#include "Client.h"
#include "v8datamodel/TimerService.h"

namespace RBX
{
	namespace Network
	{
		Reflection::PropDescriptor<Player, BrickColor> prop_teamColor("TeamColor", "Team", &Player::getTeamColor, &Player::setTeamColor, Reflection::PropertyDescriptor::STANDARD);
		Reflection::PropDescriptor<Player, bool> prop_neutral("Neutral", "Team", &Player::getNeutral, &Player::setNeutral, Reflection::PropertyDescriptor::STANDARD);
		Reflection::PropDescriptor<Player, std::string> prop_characterAppearance("CharacterAppearance", "Data", &Player::getCharacterAppearance, &Player::setCharacterAppearance, Reflection::PropertyDescriptor::STANDARD);

		Reflection::SignalDesc<Player, void(float)> event_Idled("Idled", "time");

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
	}
}
