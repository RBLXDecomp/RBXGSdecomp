#include "Client.h"
#include "Player.h"
#include "security/SecurityContext.h"
#include "v8datamodel/TimerService.h"
#include "v8datamodel/CharacterAppearance.h"
#include "v8datamodel/Accoutrement.h"
#include "v8datamodel/Hopper.h"
#include "v8datamodel/SpawnLocation.h"
#include "v8world/World.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

static RBX::Reflection::BoundFuncDesc<RBX::Network::Player, void(void), 0> loadCharacterFunction(&RBX::Network::Player::loadCharacter, "LoadCharacter", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Player, void(void), 0> removeCharacterFunction(&RBX::Network::Player::removeCharacter, "RemoveCharacter", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Player, void(bool), 1> func_SetUnder13(&RBX::Network::Player::setUnder13, "SetUnder13", "value", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Network::Player, void(bool), 1> func_SetSuperSafeChat(&RBX::Network::Player::setSuperSafeChat, "SetSuperSafeChat", "value", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

static RBX::Reflection::PropDescriptor<RBX::Network::Player, RBX::BrickColor> prop_teamColor("TeamColor", "Team", &RBX::Network::Player::getTeamColor, &RBX::Network::Player::setTeamColor, RBX::Reflection::PropertyDescriptor::STANDARD);
static RBX::Reflection::PropDescriptor<RBX::Network::Player, bool> prop_neutral("Neutral", "Team", &RBX::Network::Player::getNeutral, &RBX::Network::Player::setNeutral, RBX::Reflection::PropertyDescriptor::STANDARD);
static RBX::Reflection::PropDescriptor<RBX::Network::Player, std::string> prop_characterAppearance("CharacterAppearance", "Data", &RBX::Network::Player::getCharacterAppearance, &RBX::Network::Player::setCharacterAppearance, RBX::Reflection::PropertyDescriptor::STANDARD);

static RBX::Reflection::RefPropDescriptor<RBX::Network::Player, RBX::ModelInstance> prop_Character("Character", "Data", &RBX::Network::Player::getCharacter, &RBX::Network::Player::setCharacter, RBX::Reflection::PropertyDescriptor::STANDARD);

static RBX::Reflection::SignalDesc<RBX::Network::Player, void(float)> event_Idled("Idled", "time");

static void addChild(const boost::shared_ptr<RBX::ModelInstance>& parent, const boost::shared_ptr<RBX::Instance>& child)
{
	child->setParent(parent.get());
}

static void setAppearanceParent(boost::shared_ptr<RBX::Instance> parent, boost::shared_ptr<RBX::Instance> instance)
{
	if (dynamic_cast<RBX::CharacterAppearance*>(instance.get()))
	{
		instance->setParent(parent.get());
	}
	else if (dynamic_cast<RBX::Accoutrement*>(instance.get()))
	{
		instance->setParent(parent.get());
	}
	else
	{
		throw std::runtime_error("Attempt to add a non-CharacterAppearance/Accoutrement object to a character");
	}
}

static void setAppearanceParentNull(RBX::Instance* instance)
{
	if (dynamic_cast<RBX::CharacterAppearance*>(instance))
		instance->setParent(NULL);
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

		void Player::loadCharacterAppearance()
		{
			if (!Players::backendProcessing(this, true))
				throw std::runtime_error("LoadCharacterAppearance can only be called by the backend server");

			removeCharacterAppearance();

			if (character && !characterAppearance.empty())
			{
				std::vector<boost::shared_ptr<Instance>> characterApperance; // yes, the name's correct

				{
					std::vector<std::string> strings;
					boost::algorithm::split(strings, this->characterAppearance, boost::algorithm::is_any_of(";"));

					for (size_t i = 0; i < strings.size(); i++)
					{
						ContentProvider::singleton().load(ContentId(strings[i]), characterApperance);
					}
				}

				std::for_each(characterApperance.begin(), characterApperance.end(), boost::bind(&setAppearanceParent, character, _1));

				if (Workspace* workspace = ServiceProvider::find<Workspace>(this))
					workspace->getWorld()->update();
			}
		}

		void Player::removeCharacterAppearance()
		{
			if (!Players::backendProcessing(this, true))
				throw std::runtime_error("RemoveCharacterAppearance can only be called by the backend server");

			if (character)
				character->for_eachChild(&setAppearanceParentNull);
		}

		void Player::rebuildBackpack()
		{
			if (!Players::backendProcessing(this, true))
				throw std::runtime_error("rebuildBackpack can only be called by the backend server");

			while (Backpack* r = findFirstChildOfType<Backpack>())
			{
				r->setParent(NULL);
			}

			boost::shared_ptr<Instance> backpack = Creatable::create<Backpack>();
			backpack->setParent(this);

			StarterPackService* starterPack = ServiceProvider::find<StarterPackService>(this);
			if (starterPack)
			{
				for (size_t i = 0; i < starterPack->numChildren(); i++)
				{
					boost::shared_ptr<Instance> copy = starterPack->getChild(i)->clone();
					copy->setParent(backpack.get());
				}
			}
		}

		void Player::loadCharacter()
		{
			Workspace* workspace = ServiceProvider::find<Workspace>(this);
			if (!workspace)
				throw std::runtime_error("LoadCharacter can only be called when Player is in the world");

			if (!Players::backendProcessing(this, true))
				throw std::runtime_error("LoadCharacter can only be called by the backend server");

			std::vector<boost::shared_ptr<Instance>> instances;
			ContentProvider::singleton().load(ContentId::fromAssets("fonts\\character.rbxm"), instances);

			if (instances.size() != 1)
				throw std::runtime_error("character.rbxm should contain a Character model");

			boost::shared_ptr<ModelInstance> model = boost::shared_dynamic_cast<ModelInstance>(instances.front());
			if (!model)
				throw std::runtime_error("character.rbxm should contain a Character model");

			Humanoid* humanoid = Humanoid::modelIsCharacter(model.get());
			if (!humanoid)
				throw std::runtime_error("character.rbxm should contain a humanoid");

			{
				std::vector<boost::shared_ptr<Instance>> extraSound;
				ContentProvider::singleton().load(ContentId::fromAssets("fonts\\humanoidSound.rbxm"), extraSound);
				std::for_each(extraSound.begin(), extraSound.end(), boost::bind(&addChild, model, _1));
			}

			{
				std::vector<boost::shared_ptr<Instance>> extraHealth;
				ContentProvider::singleton().load(ContentId::fromAssets("fonts\\humanoidHealth.rbxm"), extraHealth);
				std::for_each(extraHealth.begin(), extraHealth.end(), boost::bind(&addChild, model, _1));
			}

			{
				std::vector<boost::shared_ptr<Instance>> extraAnimate;
				ContentProvider::singleton().load(ContentId::fromAssets("fonts\\humanoidAnimate.rbxm"), extraAnimate);
				std::for_each(extraAnimate.begin(), extraAnimate.end(), boost::bind(&addChild, model, _1));
			}

			model->setName(getName());
			humanoid->buildJoints();
			Instance::propArchivable.setValue(model.get(), false);
			setCharacter(model.get());
			
			model->setParent(workspace);
			loadCharacterAppearance();
			
			SpawnerService* spawnerService = ServiceProvider::find<SpawnerService>(this);
			G3D::Vector3 dropPoint = spawnerService ? spawnerService->FindSpawnPositionForPlayer(this) : G3D::Vector3(0.0f, 20.0f, 25.0f);
			workspace->moveToPoint(model.get(), dropPoint);
		}
	}
}
