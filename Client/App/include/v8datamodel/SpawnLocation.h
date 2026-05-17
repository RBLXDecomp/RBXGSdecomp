#pragma once
#include "v8datamodel/PartInstance.h"
#include "v8tree/Service.h"

namespace RBX
{
	namespace Network
	{
		class Player;
	}

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
		SpawnLocation();
	public:
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor color);
	};

	extern const char* sSpawnerService;
	class SpawnerService : public DescribedCreatable<SpawnerService, Instance, &sSpawnerService>, public Service
	{
	private:
		std::list<SpawnLocation*> spawners;

	public:
		SpawnerService();
		virtual ~SpawnerService();
		G3D::Vector3 FindSpawnPositionForPlayer(Network::Player* p);
		void RegisterSpawner(SpawnLocation* spawner);
		void UnregisterSpawner(SpawnLocation* spawner);
	};
}
