#include "v8datamodel/FlagStand.h"
#include "v8datamodel/Flag.h"
#include "humanoid/Humanoid.h"
#include "Network/Player.h"

namespace RBX
{
	char* sFlagStand = "FlagStand";

	static Reflection::PropDescriptor<FlagStand, BrickColor> prop_Color("TeamColor", "Data", &FlagStand::getTeamColor, &FlagStand::setTeamColor, Reflection::PropertyDescriptor::STANDARD);
	static Reflection::SignalDesc<FlagStand, void(boost::shared_ptr<Instance>)> event_FlagCaptured("FlagCaptured", "Player");

	FlagStand::FlagStand()
	{
		setName("FlagStand");
	}

	BrickColor FlagStand::getTeamColor() const
	{
		return teamColor;
	}

	void FlagStand::setTeamColor(BrickColor color)
	{
		teamColor = color;
		raisePropertyChanged(PartInstance::prop_Color);
	}

	void FlagStand::AffixFlag(Flag* flag)
	{
		if (!GetFlag())
		{
			flag->setParent(getParent());

			PartInstance* handle = flag->getHandle();

			handle->setCoordinateFrame(G3D::CoordinateFrame());

			G3D::Vector3 location = getLocation().translation;
			location.y += 0.5f;

			handle->moveToPoint(location);
			handle->join();
		}
	}

	Flag* FlagStand::GetFlag()
	{
		Primitive* primitive = getPrimitive();

		for (RigidJoint* current = primitive->getFirstRigid(); current != NULL; current = primitive->getNextRigid(current))
		{
			for (int i = 0; i < 2; i++)
			{
				Flag* flag = dynamic_cast<Flag*>(PartInstance::fromPrimitive(current->getPrimitive(i))->getParent());
				if (flag)
					return flag;
			}
		}

		return NULL;
	}

	bool FlagStand::HasFlag()
	{
		return GetFlag() != NULL;
	}

	void FlagStand::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Instance::onServiceProvider(oldProvider, newProvider);

		if (!oldProvider)
		{
			FlagStandService* flagStandService = ServiceProvider::create<FlagStandService>(newProvider);
			RBXASSERT(flagStandService);

			flagStandService->RegisterFlagStand(this);

			standTouched = PartInstance::event_Touched.connect(this, boost::bind(&FlagStand::onEvent_standTouched, this, _1));
		}

		if (!newProvider)
		{
			FlagStandService* flagStandService = ServiceProvider::create<FlagStandService>(oldProvider);
			RBXASSERT(flagStandService);

			flagStandService->UnregisterFlagStand(this);

			standTouched.disconnect();
		}
	}

	void FlagStand::onEvent_standTouched(boost::shared_ptr<Instance> other)
	{
		RBXASSERT(Network::Players::backendProcessing(this, true));
		Instance* parent = other->getParent();

		Humanoid* humanoid = Humanoid::modelIsCharacter(parent);
		if (!humanoid)
			return;

		Flag* flag = parent->findFirstChildOfType<Flag>();
		if (!flag)
			return;

		Network::Player* player = Network::Players::getPlayerFromCharacter(parent);
		if (!player)
			return;

		if (player->getNeutral() != false)
			return;

		BrickColor current = teamColor;
		if (player->getTeamColor() != current)
			return;

		if (flag->getTeamColor() == current)
			return;

		event_FlagCaptured.fire(this, shared_from(player));

		FlagStandService* fss = ServiceProvider::create<FlagStandService>(this);
		if (FlagStand* fs = fss->FindClosestEmptyStandForFlag(flag))
		{
			fs->AffixFlag(flag);
		}
	}

	const char* const sFlagStandService = "FlagStandService";

	FlagStandService::FlagStandService()
	{
		setName("FlagStandService");
		propArchivable.setValue(this, false);
	}

	FlagStandService::~FlagStandService() {}

	FlagStand* FlagStandService::FindStandWithFlag(Flag* f)
	{
		if (flagStands.empty())
			return NULL;

		for (std::list<FlagStand*>::const_iterator iter = flagStands.begin(); iter != flagStands.end(); iter++)
		{
			if ((*iter)->GetFlag() == f)
				return *iter;
		}

		return NULL;
	}

	void FlagStandService::RegisterFlagStand(FlagStand* s)
	{
		flagStands.push_back(s);
	}

	void FlagStandService::UnregisterFlagStand(FlagStand* s)
	{
		flagStands.remove(s);
	}

	FlagStand* FlagStandService::FindClosestEmptyStandForFlag(Flag* f)
	{
		if (flagStands.empty())
			return NULL;

		std::vector<FlagStand*> possibleLocations;
		
		for (std::list<FlagStand*>::const_iterator iter = flagStands.begin(); iter != flagStands.end(); iter++)
		{
			if ((*iter)->teamColor.asInt() == f->getTeamColor().asInt())
			{
				if (!(*iter)->HasFlag())
				{
					possibleLocations.push_back((*iter));
				}
			}
		}

		if (possibleLocations.empty())
			return NULL;

		return possibleLocations[rand() % possibleLocations.size()];
	}
}
