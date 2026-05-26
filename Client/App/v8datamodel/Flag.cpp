#include "v8datamodel/Flag.h"
#include "v8datamodel/FlagStand.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/TimerService.h"
#include "humanoid/Humanoid.h"
#include "Network/Player.h"

namespace RBX
{
	Reflection::PropDescriptor<Flag, BrickColor> prop_Color("TeamColor", "Data", &Flag::getTeamColor, &Flag::setTeamColor, Reflection::PropertyDescriptor::STANDARD);

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
		if (fastDynamicCast<PartInstance>(instance))
		{
			if (PartInstance* handle = getHandle())
			{
				boost::slot<boost::function<void(boost::shared_ptr<Instance>)>> slot(boost::bind(&Flag::onEvent_flagTouched, this, _1));
				flagTouched = PartInstance::event_Touched.connect(handle, slot);
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
}
