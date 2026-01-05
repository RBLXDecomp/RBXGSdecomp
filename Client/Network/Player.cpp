#include "Player.h"

namespace RBX
{
	namespace Network
	{
		Reflection::PropDescriptor<Player, BrickColor> prop_teamColor("TeamColor", "Team", &Player::getTeamColor, &Player::setTeamColor, Reflection::PropertyDescriptor::STANDARD);
		Reflection::PropDescriptor<Player, bool> prop_neutral("Neutral", "Team", &Player::getNeutral, &Player::setNeutral, Reflection::PropertyDescriptor::STANDARD);
		Reflection::PropDescriptor<Player, std::string> prop_characterAppearance("CharacterAppearance", "Data", &Player::getCharacterAppearance, &Player::setCharacterAppearance, Reflection::PropertyDescriptor::STANDARD);

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

				if (!character.get())
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
	}
}
