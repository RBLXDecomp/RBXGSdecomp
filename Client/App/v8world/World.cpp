#include "v8world/World.h"
#include "v8world/JointStage.h"
#include "v8world/Contact.h"
#include "util/Debug.h"
#include "v8world/ContactManager.h"
#include "v8world/SpatialHash.h"
#include "v8world/IWorldStage.h"
#include "v8world/Assembly.h"
#include "v8world/CollisionStage.h"
#include "v8world/SleepStage.h"
#include "v8world/ClumpStage.h"
#include "v8world/SimJobStage.h"
#include "v8world/JointBuilder.h"

namespace RBX
{
	bool World::disableEnvironmentalThrottle = false;

	#pragma warning (push)
	#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	World::World() : 
		contactManager(new ContactManager(this)),
		jointStage(new JointStage(NULL, this)),
		canThrottle(true),
		inStepCode(false),
		inJointNotification(false),
		worldStepId(0),
		numJoints(0),
		numContacts(0),
		numLinkCalls(0),
		profilingWorldStep(new Profiling::CodeProfiler("World Step")),
		profilingUiStep(new Profiling::CodeProfiler("UI Step")),
		profilingBroadphase(new Profiling::CodeProfiler("Broadphase"))
		{
			profilingBroadphase->parent = profilingWorldStep.get();
			profilingUiStep->parent = profilingWorldStep.get();
			getCollisionStage()->profilingCollision->parent = profilingWorldStep.get();
			getSleepStage()->profilingSleep->parent = profilingWorldStep.get();
			jointStage->getKernel()->profilingKernel->parent = profilingWorldStep.get(); 
		}
	#pragma warning (pop)

	World::~World()
	{
		RBXASSERT(numJoints == 0);
		RBXASSERT(numContacts == 0);
		RBXASSERT(primitives.size() == 0);
		RBXASSERT(breakableJoints.size() == 0);
		
		delete jointStage;
		delete contactManager;
	}

	int World::getNumContacts() const
	{
		return numContacts;
	}

	int World::getNumLinkCalls() const
	{
		return numLinkCalls;
	}

	void World::addedBodyForce() { }

	int World::getNumJoints() const {
		return numJoints;
	}

	int World::getNumPrimitives() const
	{
		return primitives.size();
	}

	Kernel& World::getKernel()
	{
		return *jointStage->getKernel();
	}

	const Kernel& World::getKernel() const
	{
		return *jointStage->getKernel();
	}

	int World::getNumBodies() const
	{
		return jointStage->getKernel()->numBodies();
	}

	int World::getNumPoints() const
	{
		return jointStage->getKernel()->numPoints();
	}

	int World::getNumConstraints() const
	{
		return jointStage->getKernel()->numConnectors();
	}

	int World::getMetric(IWorldStage::MetricType metricType) const
	{
		return jointStage->getMetric(metricType);
	}

	int World::getNumHashNodes() const
	{
		return contactManager->getSpatialHash().getNodesOut();
	}

	int World::getMaxBucketSize() const
	{
		return contactManager->getSpatialHash().getMaxBucket();
	}

	void World::onPrimitiveContactParametersChanged(Primitive* p)
	{
		for (Contact* curContact = p->getFirstContact(); curContact != NULL; curContact = p->getNextContact(curContact))
		{
			curContact->onPrimitiveContactParametersChanged();
		}
	}

	void World::onPrimitiveExtentsChanged(Primitive* p)
	{
		assertNotInStep();

		contactManager->onPrimitiveExtentsChanged(p);
	}

	void World::onPrimitiveGeometryTypeChanged(Primitive* p)
	{
		assertNotInStep();

		contactManager->onPrimitiveGeometryTypeChanged(p);
	}

	void World::onJointPrimitiveNulling(Joint* j, Primitive* p)
	{
		jointStage->onJointPrimitiveNulling(j,p);
	}

	void World::onJointPrimitiveSet(Joint* j, Primitive* p)
	{
		jointStage->onJointPrimitiveSet(j,p);
	}

	void World::insertContact(Contact* c)
	{
		jointStage->onEdgeAdded(c);
		numContacts++;
	}

	void World::destroyContact(Contact* c) 
	{
		jointStage->onEdgeRemoving(c);

		if (c)
			delete c;

		numContacts--;
	}

	void World::ticklePrimitive(Primitive* p)
	{
		Assembly* pAssembly = p->getAssembly();

		if (pAssembly)
			getSleepStage()->onWakeUpRequest(pAssembly);
	}

	void World::onPrimitiveCanSleepChanged(Primitive* p)
	{
		assertNotInStep();

		getClumpStage()->onPrimitiveCanSleepChanged(p);
	}

	void World::onPrimitiveAddedAnchor(Primitive* p)
	{
		assertNotInStep();

		getClumpStage()->onPrimitiveAddedAnchor(p);
	}

	void World::onPrimitiveRemovingAnchor(Primitive* p)
	{
		assertNotInStep();

		getClumpStage()->onPrimitiveRemovingAnchor(p);
	}

	void World::onPrimitiveCanCollideChanged(Primitive* p)
	{
		getClumpStage()->onPrimitiveCanCollideChanged(p);
	}

	void World::onAssemblyExtentsChanged(RBX::Assembly * a)
	{
		assertNotInStep();

		Assembly::PrimIterator endIt = Assembly::PrimIterator::end(a);
		Assembly::PrimIterator beginIt = Assembly::PrimIterator::begin(a);
		
		for (; beginIt != endIt; ++beginIt)
		{
			contactManager->onPrimitiveExtentsChanged(*beginIt);
		}
	}

	void World::onMotorAngleChanged(MotorJoint* m)
	{
		getClumpStage()->onMotorAngleChanged(m);
	}

	void World::update()
	{
		getClumpStage()->process();
	}

	void World::onPrimitiveTouched(Primitive* touchP, Primitive* touchOtherP)
	{
		touch.append(touchP);
		touchOther.append(touchOtherP);
	}

	void World::createJoints(Primitive *p)
	{
		createJoints(p, NULL);
	}

	void World::destroyJoints(Primitive *p)
	{
		destroyJoints(p, NULL);
	}

	void World::createJointsToWorld(const G3D::Array<Primitive*>& primitives)
	{
		assertNotInStep();

		std::set<Primitive*> ignore;
		for (int i = 0; i < primitives.size(); i++)
			ignore.insert(primitives[i]);

		for (int i = 0; i < primitives.size(); i++)
			createJoints(primitives[i], &ignore);
	}

	void World::destroyJointsToWorld(const G3D::Array<Primitive*>& primitives)
	{
		assertNotInStep();

		std::set<Primitive*> ignore;
		for (int i = 0; i < primitives.size(); i++)
			ignore.insert(primitives[i]);

		for (int i = 0; i < primitives.size(); i++)
			destroyJoints(primitives[i], &ignore);
	}

	SimJobStage& World::getSimJobStage()
	{
		return *rbx_static_cast<SimJobStage*>(jointStage->findStage(IStage::SIMJOB_STAGE));
	}

	SleepStage* World::getSleepStage()
	{
		return rbx_static_cast<SleepStage*>(jointStage->findStage(IStage::SLEEP_STAGE));
	}

	const SleepStage* World::getSleepStage() const
	{
		return rbx_static_cast<SleepStage*>(jointStage->findStage(IStage::SLEEP_STAGE));
	}

	CollisionStage* World::getCollisionStage()
	{
		return rbx_static_cast<CollisionStage*>(jointStage->findStage(IStage::COLLISION_STAGE));
	}

	const CollisionStage* World::getCollisionStage() const
	{
		return rbx_static_cast<CollisionStage*>(jointStage->findStage(IStage::COLLISION_STAGE));
	}

	ClumpStage* World::getClumpStage()
	{
		return rbx_static_cast<ClumpStage*>(jointStage->findStage(IStage::CLUMP_STAGE));
	}

	void World::removeFromBreakable(Joint* j)
	{
		if (j->isBreakable())
		{
			size_t success = breakableJoints.erase(j);
			RBXASSERT(success == 1);
		}
		else
			RBXASSERT(breakableJoints.find(j) == breakableJoints.end());
	}

	void World::removeJoint(Joint* j)
	{
		assertNotInStep();
		RBXASSERT(j);

		removeFromBreakable(j);
		jointStage->onEdgeRemoving(j);
		numJoints--;
	}

	void World::joinAll()
	{
		for (int i = 0; i < primitives.size(); i++)
			createJoints(primitives[i]);
	}

	void World::removePrimitive(Primitive* p)
	{
		assertNotInStep();
		RBXASSERT(p->inPipeline());
		RBXASSERT(p->getWorld());

		destroyJoints(p);
		contactManager->onPrimitiveRemoved(p);
		jointStage->onPrimitiveRemoving(p);
		primitives.fastRemove(p);
		p->setWorld(NULL);

		RBXASSERT(!p->getFirstEdge());
		RBXASSERT(p->getNumJoints2() + p->getNumContacts() == 0);
		assertNotInStep();
		RBXASSERT(!p->getClump());
		RBXASSERT(!p->inPipeline());
		RBXASSERT(!p->getWorld());
	}

	void World::insertPrimitive(Primitive* p)
	{
		assertNotInStep();
		RBXASSERT(!p->getClump());
		RBXASSERT(!p->inPipeline());
		RBXASSERT(!p->getFirstEdge());
		RBXASSERT(p->getNumJoints2() + p->getNumContacts() == 0);
		RBXASSERT(!p->getWorld());

		primitives.fastAppend(p);
		p->setWorld(this);
		jointStage->onPrimitiveAdded(p);
		contactManager->onPrimitiveAdded(p);

		assertNotInStep();
	}

	void World::computeFallen(G3D::Array<Primitive*>& fallen) const
	{
		typedef std::set<Assembly*>::const_iterator Iter; 

		RBXASSERT(fallen.size() != 0);

		const SleepStage* sStage = rbx_static_cast<SleepStage*>(jointStage->findStage(IStage::SLEEP_STAGE)); // World::getSleepStage()

		for (Iter cIt = sStage->getAwakeAssemblies().begin(); cIt != sStage->getAwakeAssemblies().end(); cIt++)
		{
			const Assembly* current = *cIt;
			if (current->getMainPrimitive()->getCoordinateFrame().translation.y < -500.0f)
			{
				Assembly::PrimIterator it = Assembly::PrimIterator::begin(current);
				Assembly::PrimIterator pEnd = Assembly::PrimIterator::end(current);
				for (; it != pEnd; ++it)
				{
					Primitive* currentPrim = *it;
					if (currentPrim->getCoordinateFrame().translation.y < -500.0f)
						fallen.push_back(currentPrim);
				}
			}
		}
	}

	float World::step(float desiredInterval)
	{
		assertNotInStep();

		Profiling::Mark mark(*profilingWorldStep, true);
		
		RBXASSERT(desiredInterval > 0.01);
		RBXASSERT(desiredInterval < 0.1);

		update();

		double startTick = G3D::System::getTick();
		bool throttling = false;
		int startTime = G3D::max(1, Math::iRound(floorf(desiredInterval * Constants::worldStepsPerSec())));

		for (int i = 0; i < startTime; i++)
		{
			int numOfSteps = worldStepId / Constants::worldStepsPerUiStep();

			if (worldStepId % Constants::worldStepsPerUiStep() == 0)
			{
				Profiling::Mark markUI(*profilingUiStep, false);
				doBreakJoints();
				touch.fastClear();
				touchOther.fastClear();

				inStepCode = true;
				getClumpStage()->stepUi(numOfSteps);
				getSimJobStage().notifyMovingPrimitives();
				inStepCode = false;
			}

			{
				inStepCode = true;
				Profiling::Mark markBroadphase(*profilingBroadphase, false);
				contactManager->stepWorld();
				inStepCode = false;
			}

			inStepCode = true;
			jointStage->stepWorld(worldStepId, numOfSteps, throttling);
			inStepCode = false;

			if (!World::disableEnvironmentalThrottle && canThrottle)
				throttling = G3D::System::getTick() > (i + 1) * Constants::worldDt() + startTick;
			else
				throttling = false;

			worldStepId++;
		}

		return Constants::worldDt() * startTime;
	}

	void World::insertJoint(Joint* j)
	{
		assertNotInStep();
		RBXASSERT(!inJointNotification);

		if (j->getPrimitive(0) && j->getPrimitive(1))
		{
			Joint* d = Primitive::getJoint(j->getPrimitive(0), j->getPrimitive(1));
			if (d)
			{
				inJointNotification = true;
				Notifier<World, AutoDestroy>::raise(AutoDestroy(d));
				inJointNotification = false;
			}
		}

		jointStage->onEdgeAdded(j);
		numJoints++;

		if (j->isBreakable())
		{
			size_t success = breakableJoints.insert(j).second; 
			RBXASSERT(success);
		}
	}

	void World::doBreakJoints()
	{
		std::set<Joint*>::iterator cIt = this->breakableJoints.begin();

		while (cIt != this->breakableJoints.end())
		{
			Joint* currentJoint = *cIt;
			cIt++;

			if (currentJoint->inKernel() && currentJoint->isBroken())
			{
				this->inJointNotification = true;
				Notifier<World, AutoDestroy>::raise(AutoDestroy(currentJoint));
				this->inJointNotification = false;
			}
		}
	}

	void World::createJoints(Primitive* p, std::set<Primitive*>* ignoreGroup)
	{
		assertNotInStep();
		this->numLinkCalls++;
		this->tempPrimitives.fastClear();
		this->contactManager->getPrimitivesTouchingExtents(p->getFastFuzzyExtents(), p, this->tempPrimitives);

		for (int i = 0; i < this->tempPrimitives.length(); i++)
		{
			Primitive* currentPrim = this->tempPrimitives[i];

			if (ignoreGroup == NULL || ignoreGroup->find(currentPrim) == ignoreGroup->end())
			{
				if (!Primitive::getJoint(p, currentPrim))
				{
					Joint* canJoin = JointBuilder::canJoin(p, currentPrim);
					if (canJoin)
					{
						this->insertJoint(canJoin);
						this->inJointNotification = true;
						Notifier<World, AutoJoin>::raise(AutoJoin(canJoin));
						this->inJointNotification = false;
					}
				}
			}
		}
	}

	void World::destroyJoints(Primitive* p, std::set<Primitive*>* ignoreGroup)
	{
		assertNotInStep();
		Joint* nextJoint = p->getFirstJoint();

		while (nextJoint)
		{
			Joint* prevJoint = nextJoint;
			nextJoint = p->getNextJoint(prevJoint);
			Joint::JointType jointType = prevJoint->getJointType();
			if (jointType != Joint::FREE_JOINT && jointType != Joint::ANCHOR_JOINT)
			{
				Primitive* otherPrim = prevJoint->otherPrimitive(p);
				if (ignoreGroup == NULL || ignoreGroup->find(otherPrim) == ignoreGroup->end())
				{
					this->inJointNotification = true;
					Notifier<World, AutoDestroy>::raise(AutoDestroy(prevJoint));
					this->inJointNotification = false;
				}
			}
		}
	}
}