#pragma once
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	class ServiceProvider;

	extern const char* sSpawnLocation;
	class SpawnLocation : public DescribedCreatable<SpawnLocation, PartInstance, &sSpawnLocation>
	{
	private:
		BrickColor teamColor;
		boost::signals::scoped_connection spawnerTouched;
	public:
		bool neutral;
		bool allowTeamChangeOnTouch;

	public:
		static Reflection::BoundProp<bool, 1> prop_Neutral;
		static Reflection::BoundProp<bool, 1> prop_AllowTeamChangeOnTouch;

	private:
		void onEvent_spawnerTouched(boost::shared_ptr<Instance>);
	public:
		//SpawnLocation(const SpawnLocation&);
		SpawnLocation();
	public:
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor);
	public:
		virtual ~SpawnLocation();
	public:
		//SpawnLocation& operator=(const SpawnLocation&);
	};
}
