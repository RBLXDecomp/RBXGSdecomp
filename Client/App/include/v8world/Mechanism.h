#pragma once
#include <boost/noncopyable.hpp>
#include <set>
#include <list>
#include <vector>
#include "util/Debug.h"

namespace RBX
{
	class Primitive;
	class Assembly;
	class MechanismTracker;

	class Mechanism : public boost::noncopyable
	{
	private:
		std::set<Assembly*> assemblies;
		std::vector<MechanismTracker*> trackers;
	public:
		std::list<Mechanism*>::iterator myIt;

		Mechanism::~Mechanism() 
		{ 
			RBXASSERT(this->trackers.size() == 0); 
		}

		const std::set<Assembly*>& getAssemblies() const 
		{
			return assemblies;
		}

		std::set<Assembly*>& getAssemblies() 
		{
			return assemblies;
		}

		void notifyMovingPrimitives();
		void insertAssembly(Assembly* a);
		void removeAssembly(Assembly* a);
		void absorb(Mechanism* smaller);
		Mechanism::Mechanism() {}
		static Mechanism* getMechanismFromPrimitive(const Primitive* primitive);

		friend class MechanismTracker;
	};

	class MechanismTracker
	{
	private:
		Mechanism* mechanism;
		bool containedBy(Mechanism*);
		void stopTracking();
	public:
		MechanismTracker()
			: mechanism(NULL)
		{
		}

		~MechanismTracker() 
		{
			this->stopTracking();
		}

		bool tracking();
		void setMechanism(Mechanism*);
		Mechanism* getMechanism();
		static void transferTrackers(Mechanism*, Mechanism*);
	};
}
