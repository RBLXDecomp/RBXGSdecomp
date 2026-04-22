#include "v8world/JointStage.h"
#include "v8world/Edge.h"
#include "v8world/ClumpStage.h"
#include "v8world/Primitive.h"
#include "v8world/Joint.h"

namespace RBX
{
	// TODO: determine which of these functions need to go into the header
#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	JointStage::JointStage(IStage* upstream, World* world)
		: IWorldStage(upstream, new ClumpStage(this, world), world)
	{
	}
#pragma warning (pop)

	JointStage::~JointStage()
	{
	}

	// TODO: move to header
	__forceinline bool JointStage::edgeHasPrimitivesDownstream(Edge* e)
	{
		Primitive* p0 = e->getPrimitive(0);
		Primitive* p1 = e->getPrimitive(1);
		return
			p0 &&
			p1 &&
			p0->inPipeline() &&
			p1->inPipeline() &&
			p0->downstreamOfStage(this) &&
			p1->downstreamOfStage(this);
	}

	ClumpStage* JointStage::getClumpStage()
	{
		IStage* downstream = getDownstream();
		return rbx_static_cast<ClumpStage*>(downstream);
	}

	void JointStage::moveEdgeToDownstream(Edge* e)
	{
		RBXASSERT(edgeHasPrimitivesDownstream(e));
		getClumpStage()->onEdgeAdded(e);
	}

	void JointStage::removeEdgeFromDownstream(Edge* e)
	{
		RBXASSERT(edgeHasPrimitivesDownstream(e));
		getClumpStage()->onEdgeRemoving(e);
	}

	bool JointStage::pairInMap(Joint* j, Primitive* p)
	{
		typedef std::multimap<Primitive*, Joint*>::const_iterator Iterator;

		for (Iterator it = jointMap.lower_bound(p); it != jointMap.upper_bound(p); it++)
		{
			if (it->second == j)
				return true;
		}

		return false;
	}

	void JointStage::insertToMap(Joint* j, Primitive* p)
	{
		if (!p)
			return;

		RBXASSERT(!pairInMap(j, p));
		jointMap.insert(std::pair<Primitive*, Joint*>(p, j));
	}

	void JointStage::removeFromMap(Joint* j, Primitive* p)
	{
		typedef std::multimap<Primitive*, Joint*>::iterator Iterator;

		if (!p)
			return;

		RBXASSERT(pairInMap(j, p));

		for (Iterator it = jointMap.lower_bound(p); it != jointMap.upper_bound(p); it++)
		{
			if (it->second == j) 
			{
				jointMap.erase(it);
				RBXASSERT(!pairInMap(j, p));
				return;
			}
		}

		RBXASSERT(0);
	}

	// NOTE: might be in headers
	bool JointStage::jointInList(Joint* j)
	{
		return incompleteJoints.find(j) != incompleteJoints.end();
	}

	void JointStage::removeFromList(Joint* j)
	{
		size_t count = incompleteJoints.erase(j);
		RBXASSERT(count == 1);
	}

	void JointStage::insertToList(Joint* j)
	{
		bool result = incompleteJoints.insert(j).second;
		RBXASSERT(result);
	}

	void JointStage::moveJointToDownstream(Joint* j)
	{
		RBXASSERT(!jointInList(j));
		RBXASSERT(!pairInMap(j, j->getPrimitive(0)));
		RBXASSERT(!pairInMap(j, j->getPrimitive(1)));

		moveEdgeToDownstream(j);
	}

	void JointStage::removeJointFromDownstream(Joint* j)
	{
		RBXASSERT(!jointInList(j));
		RBXASSERT(!pairInMap(j, j->getPrimitive(0)));
		RBXASSERT(!pairInMap(j, j->getPrimitive(1)));

		removeEdgeFromDownstream(j);
	}

	void JointStage::onJointPrimitiveNulling(Joint* j, Primitive* nulling)
	{
		RBXASSERT(nulling);
		RBXASSERT(j->links(nulling));
		RBXASSERT(j->downstreamOfStage(this) == edgeHasPrimitivesDownstream(j));
		RBXASSERT(j->downstreamOfStage(this) != jointInList(j));

		if (j->downstreamOfStage(this))
		{
			RBXASSERT(!pairInMap(j, nulling));
			removeJointFromDownstream(j);
			insertToList(j);
			insertToMap(j, j->otherPrimitive(nulling));
		}
		else
		{
			RBXASSERT(j->inStage(this));
			removeFromMap(j, nulling);
		}

		RBXASSERT(!pairInMap(j, nulling));
	}

	void JointStage::onJointPrimitiveSet(Joint* j, Primitive* p)
	{
		RBXASSERT(p);
		RBXASSERT(j->links(p));
		RBXASSERT(!pairInMap(j, p));
		RBXASSERT(j->inStage(this));
		RBXASSERT(jointInList(j));

		if (edgeHasPrimitivesDownstream(j))
		{
			RBXASSERT(!pairInMap(j, p));
			RBXASSERT(pairInMap(j, j->otherPrimitive(p)));

			removeFromList(j);
			removeFromMap(j, j->otherPrimitive(p));
			moveJointToDownstream(j);
		}
		else
		{
			insertToMap(j, p);
		}
	}

	void JointStage::onEdgeAdded(Edge* e)
	{
		e->putInPipeline(this);

		if (edgeHasPrimitivesDownstream(e))
		{
			moveEdgeToDownstream(e);
		}
		else
		{
			RBXASSERT(e->getEdgeType() == Edge::JOINT);
			Joint* j = rbx_static_cast<Joint*>(e);
			insertToList(j);
			insertToMap(j, j->getPrimitive(0));
			insertToMap(j, j->getPrimitive(1));
		}
	}

	void JointStage::onEdgeRemoving(Edge* e)
	{
		if (e->downstreamOfStage(this))
		{
			RBXASSERT(edgeHasPrimitivesDownstream(e));
			removeEdgeFromDownstream(e);
			e->removeFromPipeline(this);
		}
		else
		{
			RBXASSERT(e->getEdgeType() == Edge::JOINT);
			Joint* j = rbx_static_cast<Joint*>(e);
			removeFromList(j);
			removeFromMap(j, j->getPrimitive(0));
			removeFromMap(j, j->getPrimitive(1));
			e->removeFromPipeline(this);
		}
	}

	void JointStage::onPrimitiveAdded(Primitive* p)
	{
		typedef std::multimap<Primitive*, Joint*>::const_iterator Iterator;

		p->putInPipeline(this);
		getClumpStage()->onPrimitiveAdded(p);

		std::vector<Joint*> jointsToPush;

		for (Iterator it = jointMap.lower_bound(p); it != jointMap.upper_bound(p); it++)
		{
			if (edgeHasPrimitivesDownstream(it->second))
				jointsToPush.push_back(it->second);
		}

		for (size_t i = 0; i < jointsToPush.size(); i++)
		{
			Joint* current = jointsToPush[i];

			removeFromList(current);
			removeFromMap(current, p);
			removeFromMap(current, current->otherPrimitive(p));

			moveJointToDownstream(current);
		}
	}

	void JointStage::onPrimitiveRemoving(Primitive* p)
	{
		std::vector<Joint*> tempJointList;

		Joint* j;
		for (j = p->getFirstJoint(); j != NULL; j = p->getNextJoint(j))
		{
			if (j->getJointType() != Joint::ANCHOR_JOINT && j->getJointType() != Joint::FREE_JOINT)
			{
				RBXASSERT(edgeHasPrimitivesDownstream(j));
				tempJointList.push_back(j);
			}
		}

		for (size_t i = 0; i < tempJointList.size(); ++i) // why loop here???
		{
			removeJointFromDownstream(j);
			insertToList(j);
			insertToMap(j, p);

			Primitive* otherP = j->otherPrimitive(p);
			insertToMap(j, otherP);
		}

		getClumpStage()->onPrimitiveRemoving(p);
		p->removeFromPipeline(this);
	}
}