#include "v8world/Assembly.h"
#include "v8world/Primitive.h"
#include "v8world/Clump.h"
#include "v8world/Edge.h"
#include "v8world/MotorJoint.h"
#include "v8world/IMoving.h"

namespace RBX
{
	// TODO: check which of these are meant for the header

	void Assembly::putInKernel(Kernel* _kernel)
	{
		IPipelined::putInKernel(_kernel);
		rootClump->putInKernel(_kernel);
	}

	void Assembly::removeFromKernel()
	{
		RBXASSERT(getKernel());
		rootClump->removeFromKernel(getKernel());
		IPipelined::removeFromKernel();
	}

	Primitive* Assembly::getMainPrimitive() const
	{
		return rootClump->getRootPrimitive();
	}

	bool Assembly::getAnchored() const
	{
		return rootClump->getAnchored();
	}

	int Assembly::getDof() const
	{
		return getAnchored() ? 0 : 6;
	}

	Sim::AssemblyState Assembly::getSleepStatus()
	{
		return rootClump->getSleepStatus();
	}

	bool Assembly::moving()
	{
		return !getAnchored() && getSleepStatus() == Sim::AWAKE;
	}

	int Assembly::getSleepCount() const
	{
		return sleepCount;
	}

	void Assembly::setSleepCount(int count)
	{
		sleepCount = count;
	}

	void Assembly::incrementSleepCount()
	{
		++sleepCount;
	}

	Mechanism* Assembly::getMechanism()
	{
		return mechanism;
	}

	bool Assembly::calcShouldSleep()
	{
		return rootClump->calcShouldSleep();
	}

	bool Assembly::okNeighborSleep()
	{
		return rootClump->okNeighborSleep();
	}

	bool Assembly::forceNeighborAwake()
	{
		return rootClump->forceNeighborAwake();
	}

	void Assembly::setSleepStatus(Sim::AssemblyState _set)
	{
		rootClump->setSleepStatus(_set);
	}

	void Assembly::onPrimitiveCanSleepChanged(Primitive* p)
	{
		rootClump->onPrimitiveCanSleepChanged(p);
	}

	Assembly* Assembly::otherAssembly(Edge* edge) const
	{
		Assembly* a1 = edge->getPrimitive(0)->getAssembly();
		Assembly* a2 = edge->getPrimitive(1)->getAssembly();
		return a1 != this ? a1 : a2;
	}

	size_t Assembly::numMotors() const
	{
		return motors.size();
	}

	bool Assembly::getCanSleep()
	{
		return !Primitive::disableSleep && rootClump->getCanSleep() && Assembly::numMotors() == 0;
	}

	Primitive* Assembly::PrimIterator::operator*() const
	{
		return *primIterator;
	}

	Assembly::PrimIterator Assembly::PrimIterator::begin(const Assembly* assembly)
	{
		const std::set<Clump*>& clumps = assembly->getClumps();
		const std::set<Clump*>::const_iterator start = clumps.begin();

		RBXASSERT(start != clumps.end());
		Clump* clump = *start;

		return PrimIterator(assembly, start, clump->clumpPrimBegin());
	}

	Assembly::PrimIterator Assembly::PrimIterator::end(const Assembly* assembly)
	{
		const std::set<Clump*>& clumps = assembly->getClumps();
		std::set<Clump*>::const_iterator end = clumps.end();

		end--;
		Clump* clump = *end;

		return PrimIterator(assembly, end, clump->clumpPrimEnd());
	}

	void Assembly::stepUi(int uiStepId)
	{
		typedef std::vector<MotorJoint*>::const_iterator Iterator;

		for (Iterator iter = motors.begin(); iter != motors.end(); iter++)
		{
			(*iter)->stepUi(uiStepId);
		}
	}

	Assembly::PrimIterator& Assembly::PrimIterator::operator++()
	{
		RBXASSERT(clumpIterator != assembly->getClumps().end());

		Clump* clump = *clumpIterator;
		primIterator++;

		if (primIterator == clump->clumpPrimEnd())
		{
			clumpIterator++;
			if (clumpIterator == assembly->getClumps().end())
			{
				*this = end(assembly);
			}
			else
			{
				primIterator = (*clumpIterator)->clumpPrimBegin();
				RBXASSERT(primIterator != (*clumpIterator)->clumpPrimEnd());
			}
		}

		return *this;
	}

	void Assembly::notifyMoved()
	{
		const Assembly::PrimIterator itEnd = Assembly::PrimIterator::end(this);
		Assembly::PrimIterator it = Assembly::PrimIterator::begin(this);

		// TODO: is operator!= right?
		for (; it != itEnd; ++it)
		{
			(*it)->getOwner()->notifyMoved();
		}
	}

	void Assembly::removeMotor(MotorJoint* m)
	{
		typedef std::vector<MotorJoint*>::iterator Iterator;
		Iterator iter = std::find(motors.begin(), motors.end(), m);

		RBXASSERT(iter != motors.end());
		motors.erase(iter);
	}

	bool Assembly::containsExternalEdge(Edge* e)
	{
		return externalEdges.find(e) != externalEdges.end();
	}

	bool Assembly::containsInternalEdge(Edge* e)
	{
		return internalEdges.find(e) != internalEdges.end();
	}

	void Assembly::insertClump(Clump* c)
	{
		bool success = clumps.insert(c).second;
		RBXASSERT(success);
		c->setAssembly(this);
	}

	void Assembly::addExternalEdge(Edge* e)
	{
		bool success = externalEdges.insert(e).second;
		RBXASSERT(success);
	}

	void Assembly::addInternalEdge(Edge* e)
	{
		bool success = internalEdges.insert(e).second;
		RBXASSERT(success);
	}

	void Assembly::addInconsistentMotor(MotorJoint* m)
	{
		bool success = inconsistentMotors.insert(m).second;
		RBXASSERT(success);
	}

	Assembly::~Assembly()
	{
		RBXASSERT(!mechanism);
		RBXASSERT(clumps.empty());
		RBXASSERT(motors.empty());
		RBXASSERT(inconsistentMotors.empty());
		RBXASSERT(externalEdges.empty());
		RBXASSERT(internalEdges.empty());
	}

	void Assembly::removeClump(Clump* c)
	{
		if (c == rootClump)
			rootClump = NULL;
		c->setAssembly(NULL);
		c->getRootPrimitive()->getBody()->setParent(NULL);

		size_t removed = clumps.erase(c);
		RBXASSERT(removed == 1);
	}

	void Assembly::removeExternalEdge(Edge* e)
	{
		size_t removed = externalEdges.erase(e);
		RBXASSERT(removed == 1);
	}

	void Assembly::removeInternalEdge(Edge* e)
	{
		size_t removed = internalEdges.erase(e);
		RBXASSERT(removed == 1);
	}

	void Assembly::removeInconsistentMotor(MotorJoint* m)
	{
		size_t removed = inconsistentMotors.erase(m);
		RBXASSERT(removed == 1);
	}

	Assembly::Assembly(Clump* root)
		: IPipelined(),
		  rootClump(root),
		  clumps(),
		  internalEdges(),
		  externalEdges(),
		  motors(),
		  inconsistentMotors(),
		  mechanism(NULL),
		  sleepCount(0)
	{
		insertClump(root);
	}

	// TODO: check if this matches
	bool lessMotorHash(const MotorJoint* m0, const MotorJoint* m1)
	{
		return m0->hashCode() < m1->hashCode();
	}

	const MotorJoint* Assembly::getMotorConst(unsigned int motorId) const
	{
		if (motorId >= motors.size())
			return NULL;

		std::sort(motors.begin(), motors.end(), lessMotorHash);
		return motors[motorId];
	}

	MotorJoint* Assembly::getMotor(unsigned int motorId)
	{
		return const_cast<MotorJoint*>(getMotorConst(motorId));
	}

	void Assembly::addClump(Clump* c, MotorJoint* m)
	{
		insertClump(c);

		Primitive* p = (m->getPrimitive(0)->getClump() == c) ? m->getPrimitive(0) : m->getPrimitive(1);
		Primitive* other = m->otherPrimitive(p);

		RBXASSERT(clumps.find(other->getClump()) != clumps.end());

		c->getRootPrimitive()->getBody()->setParent(other->getBody());
		RBXASSERT(std::find(motors.begin(), motors.end(), m) == motors.end());
		motors.push_back(m);
	}
}
