#include "v8datamodel/Flag.h"
#include "v8datamodel/FlagStand.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/TimerService.h"
#include "humanoid/Humanoid.h"
#include "Network/Player.h"

namespace RBX
{
	const char* const sFlag = "Flag";

	static Reflection::PropDescriptor<Flag, BrickColor> prop_Color("TeamColor", "Data", &Flag::getTeamColor, &Flag::setTeamColor, Reflection::PropertyDescriptor::STANDARD);

	Flag::Flag()
		: timer(NULL)
	{
		setName("Flag");
	}

	Flag::~Flag() {}

	BrickColor Flag::getTeamColor() const
	{
		return teamColor;
	}

	void Flag::setTeamColor(BrickColor color)
	{
		teamColor = color;
		raisePropertyChanged(prop_Color);
	}

	void Flag::onChildAdded(Instance* instance)
	{
		if (dynamic_cast<PartInstance*>(instance))
		{
			if (PartInstance* handle = getHandle())
			{
				flagTouched = PartInstance::event_Touched.connect(handle, boost::bind(&Flag::onEvent_flagTouched, this, _1));
			}
		}
	}

	void Flag::doUglyPeriodicCloneHack()
	{
		evilClone = clone();

		if (timer && numChildren() == 0)
		{
			timer->delay(boost::bind(&Flag::doUglyPeriodicCloneHack, shared_from(this)), 2.0f);
		}
	}

	bool Flag::canBePickedUpByPlayer(Network::Player* p)
	{
		if (p->getNeutral() != false)
			return false;

		BrickColor current = teamColor;
		if (p->getTeamColor() != current)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Flag::onEvent_flagTouched(boost::shared_ptr<Instance> other)
	{
		RBXASSERT(Network::Players::backendProcessing(this, true));
		Instance* parent = other->getParent();

		Humanoid* humanoid = Humanoid::modelIsCharacter(parent);
		if (!humanoid)
			return;

		Network::Player* player = Network::Players::getPlayerFromCharacter(parent);
		if (!player)
			return;

		if (player->getNeutral() != false)
			return;

		BrickColor current = teamColor;
		if (player->getTeamColor() != current)
			return;

		FlagStandService* fss = ServiceProvider::create<FlagStandService>(this);
		if (!fss->FindStandWithFlag(this))
		{
			if (FlagStand* fs = fss->FindClosestEmptyStandForFlag(this))
			{
				fs->AffixFlag(this);
			}
		}
	}

	void Flag::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Instance::onServiceProvider(oldProvider, newProvider);

		if (!oldProvider)
		{
			timer = ServiceProvider::create<TimerService>(newProvider);
			timer->delay(boost::bind(&Flag::doUglyPeriodicCloneHack, shared_from(this)), 2.0f);
		}

		if (!newProvider)
		{
			flagTouched.disconnect();
			RunService* runService = ServiceProvider::create<RunService>(oldProvider);
			RBXASSERT(runService);

			if (runService->getRunState() == RS_RUNNING)
			{
				if (Instance* evilC = evilClone.get())
				{
					Flag* flag = dynamic_cast<Flag*>(evilC);
					PartInstance* handle = flag->getHandle();

					FlagStandService* fss = ServiceProvider::create<FlagStandService>(this);
					if (!fss->FindStandWithFlag(flag))
					{
						if (FlagStand* fs = fss->FindClosestEmptyStandForFlag(flag))
						{
							fs->AffixFlag(flag);
						}
					}
				}
			}
		}
	}
}
