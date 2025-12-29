#include "v8world/SleepStage.h"
#include "v8world/Assembly.h"
#include "v8world/Edge.h"
#include "v8world/Primitive.h"
#include "v8world/SeparateStage.h"
#include "v8world/CollisionStage.h"

namespace RBX
{
#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	SleepStage::SleepStage(IStage* upstream, World* world)
		: IWorldStage(upstream, new SeparateStage(this, world), world),
		  profilingSleep(new Profiling::CodeProfiler("Sleep"))
	{
	}
#pragma warning (pop)

	SleepStage::~SleepStage()
	{
		RBXASSERT(awake.empty());
		RBXASSERT(sleepingChecking.empty());
		RBXASSERT(sleepingDeeply.empty());
	}

	int SleepStage::stepsToSleep()
	{
		return 20;
	}

	std::set<Assembly*>& SleepStage::statusToArray(Sim::AssemblyState status)
	{
		if (status == Sim::AWAKE)
			return awake;
		else if (status == Sim::SLEEPING_CHECKING)
			return sleepingChecking;
		else
			return sleepingDeeply;
	}

	CollisionStage* SleepStage::getCollisionStage()
	{
		return rbx_static_cast<CollisionStage*>(getUpstreamWS());
	}

	void SleepStage::stepWorld(int worldStepId, int uiStepId, bool throttling)
	{
		{
			Profiling::Mark mark(*profilingSleep.get(), false);

			checkAwakeAssemblies(throttling);
			if (worldStepId % 4 == 0)
				checkSleepingAssemblies();
		}

		RBXASSERT(getDownstream());
		getDownstream()->stepWorld(worldStepId, uiStepId, throttling);
	}

	void SleepStage::remove(Assembly* assembly)
	{
		if (assembly->getSleepStatus() == Sim::AWAKE)
		{
			RBXASSERT(assembly->downstreamOfStage(this));
			rbx_static_cast<SeparateStage*>(getDownstreamWS())->onAssemblyRemoving(assembly);
		}

		RBXASSERT(assembly->inStage(this));

		std::set<Assembly*>& assemblyArray = statusToArray(assembly->getSleepStatus());
		size_t removed = assemblyArray.erase(assembly);
		RBXASSERT(removed == 1);

		assembly->setSleepStatus(Sim::AWAKE);
		assembly->setSleepCount(0);
	}

	void SleepStage::insert(Assembly* assembly, Sim::AssemblyState newStatus)
	{
		RBXASSERT(assembly->getSleepCount() == 0);
		RBXASSERT(assembly->getSleepStatus() == Sim::AWAKE);
		RBXASSERT(!assembly->downstreamOfStage(this));

		std::set<Assembly*>& assemblyArray = statusToArray(newStatus);
		bool inserted = assemblyArray.insert(assembly).second;
		RBXASSERT(inserted);

		assembly->setSleepStatus(newStatus);
		if (assembly->getSleepStatus() == Sim::AWAKE)
		{
			rbx_static_cast<SeparateStage*>(getDownstreamWS())->onAssemblyAdded(assembly);
		}
	}

	void SleepStage::changeSleepStatus(Assembly* assembly, Sim::AssemblyState newStatus)
	{
		remove(assembly);
		insert(assembly, newStatus);
	}

	void SleepStage::onAssemblyAdded(Assembly* assembly)
	{
		RBXASSERT(!assembly->getAnchored());
		RBXASSERT(assembly->getSleepStatus() == Sim::AWAKE);

		assembly->putInStage(this);
		insert(assembly, Sim::AWAKE);
	}

	void SleepStage::onAssemblyRemoving(Assembly* assembly)
	{
		RBXASSERT(assembly->inOrDownstreamOfStage(this));

		remove(assembly);
		RBXASSERT(assembly->getSleepStatus() == Sim::AWAKE);
		assembly->removeFromStage(this);
	}

	void SleepStage::onEdgeAdded(Edge* e)
	{
		RBXASSERT(!e->inOrDownstreamOfStage(this));

		Assembly* a0 = e->getPrimitive(0)->getAssembly();
		Assembly* a1 = e->getPrimitive(1)->getAssembly();

		RBXASSERT(!a0->getAnchored() || !a1->getAnchored());
		RBXASSERT(a0->inOrDownstreamOfStage(this) || a1->inOrDownstreamOfStage(this));

		bool a0Awoken = false;
		bool a1Awoken = false;

		if (!a0->getAnchored() && a0->getSleepStatus() != Sim::AWAKE)
		{
			wakeAssembly(a0);
			a0Awoken = true;
		}

		if (!a1->getAnchored() && a1->getSleepStatus() != Sim::AWAKE)
		{
			wakeAssembly(a1);
			a1Awoken = true;
		}

		IWorldStage::onEdgeAdded(e);

		if (a0Awoken)
			wakeAssemblyAndNeighbors(a0, 8);

		if (a1Awoken)
			wakeAssemblyAndNeighbors(a1, 8);
	}

	void SleepStage::onEdgeRemoving(Edge* e)
	{
		RBXASSERT(e->inOrDownstreamOfStage(this));

		if (e->downstreamOfStage(this))
			getDownstreamWS()->onEdgeRemoving(e);

		Assembly* a0 = e->getPrimitive(0)->getAssembly();
		Assembly* a1 = e->getPrimitive(1)->getAssembly();
		RBXASSERT(a0->inOrDownstreamOfStage(this) || a1->inOrDownstreamOfStage(this));

		e->removeFromStage(this);

		if (a0->getSleepStatus() != Sim::AWAKE)
			wakeAssemblyAndNeighbors(a0, 8);
		if (a1->getSleepStatus() != Sim::AWAKE)
			wakeAssemblyAndNeighbors(a1, 8);
	}

	bool SleepStage::shouldSleep(Assembly* assembly)
	{
		RBXASSERT(assembly->getSleepStatus() == Sim::AWAKE);
		if (!assembly->getCanSleep())
			return false;
		if (!assembly->calcShouldSleep())
			return false;

		typedef std::set<Edge*>::iterator Iterator;
		for (Iterator it = assembly->getExternalEdges().begin(); it != assembly->getExternalEdges().end(); it++)
		{
			Edge* e = *it;
			if (e->inOrDownstreamOfStage(this))
			{
				Assembly* otherAssembly = assembly->otherAssembly(e);
				if (otherAssembly->getSleepStatus() == Sim::AWAKE &&
					!otherAssembly->getAnchored() &&
					!otherAssembly->okNeighborSleep())
					return false;
			}
		}
		return true;
	}

	Sim::AssemblyState SleepStage::shouldWakeOrSleepDeeply(Assembly* assembly)
	{
		RBXASSERT(assembly->getSleepStatus() == Sim::SLEEPING_CHECKING);
		if (!assembly->getCanSleep())
			return Sim::AWAKE;

		typedef std::set<Edge*>::iterator Iterator;
		bool deep = true;

		for (Iterator it = assembly->getExternalEdges().begin(); it != assembly->getExternalEdges().end(); it++)
		{
			Edge* e = *it;
			if (e->inOrDownstreamOfStage(this))
			{
				Assembly* otherAssembly = assembly->otherAssembly(e);
				if (otherAssembly->getSleepStatus() == Sim::AWAKE && !otherAssembly->getAnchored())
				{
					deep = false;
					if (otherAssembly->forceNeighborAwake())
						return Sim::AWAKE;
				}
			}
		}
		return deep ? Sim::SLEEPING_DEEPLY : Sim::SLEEPING_CHECKING;
	}

	void SleepStage::goToSleep(Assembly* assembly)
	{
		RBXASSERT(assembly->getSleepStatus() == Sim::AWAKE);

		typedef std::set<Edge*>::iterator Iterator;
		for (Iterator it = assembly->getExternalEdges().begin(); it != assembly->getExternalEdges().end(); it++)
		{
			Edge* e = *it;
			if (e->downstreamOfStage(this))
			{
				Assembly* otherAssembly = assembly->otherAssembly(e);
				if (!otherAssembly->moving())
				{
					getDownstreamWS()->onEdgeRemoving(e);
				}
			}
		}

		changeSleepStatus(assembly, Sim::SLEEPING_CHECKING);

		getCollisionStage()->onSleepChanged(assembly);
	}

	void SleepStage::wakeAssembly(Assembly* assembly)
	{
		RBXASSERT(assembly->getSleepStatus() != Sim::AWAKE);
		RBXASSERT(!assembly->getAnchored());
		RBXASSERT(assembly->inOrDownstreamOfStage(this));

		changeSleepStatus(assembly, Sim::AWAKE);
	}

	void SleepStage::wakeAssemblyAndNeighbors(Assembly* assembly, int recurseDepth)
	{
		if (assembly->getAnchored())
			return;

		if (assembly->getSleepStatus() != Sim::AWAKE)
			wakeAssembly(assembly);

		typedef std::set<Edge*>::iterator Iterator;
		for (Iterator it = assembly->getExternalEdges().begin(); it != assembly->getExternalEdges().end(); it++)
		{
			Edge* e = *it;
			if (e->inStage(this))
			{
				if (!e->downstreamOfStage(this))
				{
					getDownstreamWS()->onEdgeAdded(e);
				}

				Assembly* otherAssembly = assembly->otherAssembly(e);
				if (recurseDepth > 0)
				{
					if (otherAssembly->getSleepStatus() != Sim::AWAKE)
					{
						wakeAssemblyAndNeighbors(otherAssembly, recurseDepth - 1);
					}
				}
				else
				{
					if (otherAssembly->getSleepStatus() == Sim::SLEEPING_DEEPLY)
					{
						changeSleepStatus(otherAssembly, Sim::SLEEPING_CHECKING);
					}
				}
			}
		}

		getCollisionStage()->onSleepChanged(assembly);
	}

	void SleepStage::onLosingContact(const G3D::Array<Contact*>& separating)
	{
		for (int i = 0; i < separating.size(); ++i)
		{
			Contact* c = separating[i];

			Assembly* a0 = c->getPrimitive(0)->getAssembly();
			Assembly* a1 = c->getPrimitive(1)->getAssembly();

			RBXASSERT(a0->inOrDownstreamOfStage(this) || a1->inOrDownstreamOfStage(this));

			if (a0->getSleepStatus() != Sim::AWAKE)
				wakeAssemblyAndNeighbors(a0, 8);
			if (a1->getSleepStatus() != Sim::AWAKE)
				wakeAssemblyAndNeighbors(a1, 8);
		}

		for (int i = 0; i < separating.size(); ++i)
		{
			Contact* c = separating[i];

			RBXASSERT(c->inOrDownstreamOfStage(this));
			if (c->downstreamOfStage(this))
			{
				getDownstreamWS()->onEdgeRemoving(c);
			}

			RBXASSERT(c->inStage(this));
			c->removeFromStage(this);
		}

		getCollisionStage()->onLosingContact(separating);
	}

	void SleepStage::onWakeUpRequest(Assembly* assembly)
	{
		if (assembly->inPipeline() && assembly->inOrDownstreamOfStage(this) && assembly->getSleepStatus() != Sim::AWAKE)
			wakeAssemblyAndNeighbors(assembly, 8);
	}

	void SleepStage::checkAwakeAssemblies(bool throttling)
	{
		G3D::Array<Assembly*> tempToSleep;

		typedef std::set<Assembly*>::iterator Iterator;
		for (Iterator it = awake.begin(); it != awake.end(); it++)
		{
			Assembly* assembly = *it;
			RBXASSERT(!assembly->getAnchored());

			if (!throttling || !assembly->getMainPrimitive()->getBody()->getCanThrottle())
			{
				if (shouldSleep(assembly))
				{
					assembly->incrementSleepCount();
					if (assembly->getSleepCount() > stepsToSleep())
					{
						assembly->setSleepCount(0);
						tempToSleep.append(assembly);
					}
				}
				else
				{
					assembly->setSleepCount(0);
				}
			}
		}

		for (int i = 0; i < tempToSleep.size(); ++i)
		{
			goToSleep(tempToSleep[i]);
		}
	}

	void SleepStage::checkSleepingAssemblies()
	{
		G3D::Array<Assembly*> tempToWake;
		G3D::Array<Assembly*> tempToDeep;

		typedef std::set<Assembly*>::iterator Iterator;
		for (Iterator it = sleepingChecking.begin(); it != sleepingChecking.end(); it++)
		{
			Assembly* assembly = *it;
			RBXASSERT(!assembly->getAnchored());
			RBXASSERT(assembly->getSleepCount() == 0);

			Sim::AssemblyState state = shouldWakeOrSleepDeeply(assembly);

			if (state != Sim::AWAKE)
			{
				if (state == Sim::SLEEPING_DEEPLY)
					tempToDeep.append(assembly);
			}
			else
			{
				tempToWake.append(assembly);
			}
		}

		for (int i = 0; i < tempToWake.size(); ++i)
		{
			Assembly* assembly = tempToWake[i];
			wakeAssemblyAndNeighbors(assembly, 0);
		}

		for (int i = 0; i < tempToDeep.size(); ++i)
		{
			Assembly* assembly = tempToDeep[i];
			changeSleepStatus(assembly, Sim::SLEEPING_DEEPLY);
		}
	}
}
