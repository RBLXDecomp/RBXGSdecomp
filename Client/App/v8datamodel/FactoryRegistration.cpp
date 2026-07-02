#include "v8datamodel/FactoryRegistration.h"
#include "v8datamodel/Lighting.h"
#include "v8datamodel/Accoutrement.h"
#include "v8datamodel/Backpack.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/CharacterAppearance.h"
#include "v8datamodel/DebrisService.h"
#include "v8datamodel/Explosion.h"
#include "v8datamodel/Feature.h"
#include "v8datamodel/Flag.h"
#include "v8datamodel/FlagStand.h"
#include "v8datamodel/GameSettings.h"
#include "v8datamodel/GlobalSettings.h"
#include "v8datamodel/Gyro.h"
#include "v8datamodel/Hopper.h"
#include "v8datamodel/JointInstance.h"
#include "v8datamodel/LocalBackpack.h"
#include "v8datamodel/Message.h"
#include "v8datamodel/Seat.h"
#include "v8datamodel/Selection.h"
#include "v8datamodel/Sky.h"
#include "v8datamodel/SpawnLocation.h"
#include "v8datamodel/Stats.h"
#include "v8datamodel/Team.h"
#include "v8datamodel/Teams.h"
#include "v8datamodel/TimerService.h"
#include "v8datamodel/UserController.h"
#include "v8datamodel/Value.h"
#include "v8datamodel/Visit.h"
#include "humanoid/Humanoid.h"
#include "script/Script.h"
#include "util/Sound.h"

namespace RBX
{
	FactoryRegistrator::FactoryRegistrator()
	{
		Accoutrement::classDescriptor();
		Backpack::classDescriptor();
		BodyColors::classDescriptor();
		ControllerService::classDescriptor();
		Explosion::classDescriptor();
		Flag::classDescriptor();
		FlagStand::classDescriptor();
		FlagStandService::classDescriptor();
		GameSettings::classDescriptor();
		GlobalSettings::classDescriptor();
		Hat::classDescriptor();
		Hint::classDescriptor();
		Humanoid::classDescriptor();
		Instance::classDescriptor();
		LegacyHopperService::classDescriptor();
		LocalBackpack::classDescriptor();
		LocalBackpackItem::classDescriptor();
		LocalScript::classDescriptor();
		Message::classDescriptor();
		Selection::classDescriptor();
		ShirtGraphic::classDescriptor();
		StarterPackService::classDescriptor();
		Stats::StatsService::classDescriptor();
		Visit::classDescriptor();
		Seat::classDescriptor();
		DebrisService::classDescriptor();
		TimerService::classDescriptor();
		SpawnerService::classDescriptor();

		//all of the joints
		Glue::classDescriptor();
		Motor::classDescriptor();
		Rotate::classDescriptor();
		RotateP::classDescriptor();
		RotateV::classDescriptor();
		Snap::classDescriptor();
		Weld::classDescriptor();

		MotorFeature::classDescriptor();
		Hole::classDescriptor();

		registerSound();
		registerBodyMovers();

		registerValueClasses();
	}
}
