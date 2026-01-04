#pragma once
#include <set>
#include <boost/scoped_ptr.hpp>
#include <G3D/Array.h>
#include "v8world/IWorldStage.h"
#include "util/Profiling.h"

namespace RBX
{
	namespace Sim
	{
		enum AssemblyState;
	}

	class Assembly;
	class CollisionStage;
	class World;
	class Edge;
	class Contact;

	class SleepStage : public IWorldStage
	{
	private:
		std::set<Assembly*> awake;
		std::set<Assembly*> sleepingChecking;
		std::set<Assembly*> sleepingDeeply;
	public:
		boost::scoped_ptr<Profiling::CodeProfiler> profilingSleep;
  
	private:
		std::set<Assembly*>& statusToArray(Sim::AssemblyState status);
		void remove(Assembly* assembly);
		void insert(Assembly* assembly, Sim::AssemblyState newStatus);
		void changeSleepStatus(Assembly* assembly, Sim::AssemblyState newStatus);
		bool shouldSleep(Assembly* assembly);
		Sim::AssemblyState shouldWakeOrSleepDeeply(Assembly* assembly);
		void wakeAssembly(Assembly* assembly);
		void goToSleep(Assembly* assembly);
		void wakeAssemblyAndNeighbors(Assembly*, bool);
		void wakeAssemblyAndNeighbors(Assembly* assembly, int recurseDepth);
		void checkAwakeAssemblies(bool throttling);
		void checkSleepingAssemblies();
		void validateEdge(Edge*);
		bool debugValidate();
		CollisionStage* getCollisionStage();
	public:
		//SleepStage(const SleepStage&);
		SleepStage(IStage* upstream, World* world);
		virtual ~SleepStage();
	public:
		virtual StageType getStageType()
		{
			return SLEEP_STAGE;
		}
		virtual void stepWorld(int worldStepId, int uiStepId, bool throttling);
		virtual void onEdgeAdded(Edge* e);
		virtual void onEdgeRemoving(Edge* e);
		void onAssemblyAdded(Assembly* assembly);
		void onAssemblyRemoving(Assembly* assembly);
		void onWakeUpRequest(Assembly* assembly);
		int numTouchingContacts();
		const std::set<Assembly*>& getAwakeAssemblies() const
		{
			return awake;
		}
		void onLosingContact(const G3D::Array<Contact*>& separating);
		//SleepStage& operator=(const SleepStage&);
  
	private:
		static int stepsToSleep();
	};
}