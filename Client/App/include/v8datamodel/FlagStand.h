#pragma once
#include "v8datamodel/PartInstance.h"
#include "v8tree/Service.h"

namespace RBX
{
	class Flag;

	extern const char* sFlagStand;

	class FlagStand : public DescribedCreatable<FlagStand, PartInstance, &sFlagStand>
	{
	private:
		boost::signals::scoped_connection standTouched;
	public:
		BrickColor teamColor;

	private:
		void onEvent_standTouched(boost::shared_ptr<Instance>);
	public:
		FlagStand();
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		void AffixFlag(Flag*);
		bool HasFlag();
		Flag* GetFlag();
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor);
	};

	extern const char* sFlagStandService;

	class FlagStandService : public DescribedCreatable<FlagStandService, Instance, &sFlagStandService>, public Service
	{
	private:
		std::list<FlagStand*> flagStands;

	public:
		FlagStandService();
		virtual ~FlagStandService();
		FlagStand* FindClosestEmptyStandForFlag(Flag*);
		FlagStand* FindStandWithFlag(Flag*);
		void RegisterFlagStand(FlagStand*);
		void UnregisterFlagStand(FlagStand*);
	};
}
