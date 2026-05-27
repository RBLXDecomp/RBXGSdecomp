#include "v8datamodel/FlagStand.h"
#include "v8datamodel/Flag.h"

namespace RBX
{
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
				Flag* flag = fastDynamicCast<Flag>(PartInstance::fromPrimitive(current->getPrimitive(i))->getParent());
				if (flag)
					return flag;
			}
		}

		return NULL;
	}

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
}
