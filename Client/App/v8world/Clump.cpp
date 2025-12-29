#include "v8world/Clump.h"
#include "v8world/Primitive.h"
#include "v8world/RigidJoint.h"
#include "v8world/Anchor.h"
#include "v8world/Assembly.h"
#include <deque>

namespace RBX
{
	// TODO: check which of these are meant for the header

	/*__forceinline*/ Primitive* SpanLink::isSpanningJoint(RigidJoint* r)
	{
		Primitive* prim0 = r->getPrimitive(0);
		Primitive* prim1 = r->getPrimitive(1);
		const Body* body0 = prim0->getBody();
		const Body* body1 = prim1->getBody();

		if (body1->getParent() == body0)
		{
			RBXASSERT(body0->getParent() != body1);
			RBXASSERT(prim0->getClump() == prim1->getClump());
			return prim0;
		}
		else if (body0->getParent() == body1)
		{
			RBXASSERT(prim0->getClump() == prim1->getClump());
			return prim1;
		}

		return NULL;
	}

	bool SpanLink::isDownstreamSpanningJoint(Primitive* p, RigidJoint* r)
	{
		Primitive* prim = isSpanningJoint(r);
		return prim && prim == p;
	}

	SpanLink SpanLink::bestSpanLink(const SpanLink& s0, const SpanLink& s1)
	{
		Primitive* p0 = s0.parent;
		Primitive* p1 = s1.parent;

		if (p0)
		{
			if (p1)
			{
				int p0cd = p0->getClumpDepth();
				int p1cd = p1->getClumpDepth();
				if (p0cd != p1cd)
				{
					return p0cd > p1cd ? s0 : s1;
				}
				else
				{
					float p0ps = p0->getPlanarSize();
					float p1ps = p1->getPlanarSize();

					return (p0ps > p1ps) ? s0 : s1;
				}
			}
			else
			{
				return s0;
			}
		}
		else
		{
			return s1;
		}
	}

	Clump* Clump::otherClump(Edge* e) const
	{
		return e->getPrimitive(0)->getClump() != this ? e->getPrimitive(0)->getClump() : e->getPrimitive(1)->getClump();
	}

	void Clump::addAnchor(Anchor* a)
	{
		RBXASSERT(!anchor);
		RBXASSERT(a->getPrimitive() == rootPrimitive);

		anchor = a;
	}

	Anchor* Clump::removeAnchor()
	{
		RBXASSERT(anchor);

		Anchor* a = anchor;
		anchor = NULL;
		return a;
	}

	bool Clump::inSpanTree(Primitive* p, Primitive* treeRoot)
	{
		Body* treeBody = treeRoot->getBody();
		Body* body = p->getBody();

		while (body)
		{
			if (body == treeBody)
				return true;
			body = body->getParent();
		}
		return false;
	}

	// 66% match if isSpanningJoint has __forceinline
	SpanLink Clump::findBestOutsideLink(Primitive* treeRoot, Primitive* current)
	{
		SpanLink bestChildLink;

		for (RigidJoint* r = current->getFirstRigid(); r != NULL; r = current->getNextRigid(r))
		{
			Primitive* other = r->otherPrimitive(current);
			Primitive* prim = SpanLink::isSpanningJoint(r);

			if (prim)
			{
				if (prim == current)
				{
					bestChildLink = SpanLink::bestSpanLink(bestChildLink, findBestOutsideLink(treeRoot, other));
				}
			}
			else
			{
				if (other->getClump() == current->getClump())
				{
					if (inSpanTree(other, treeRoot))
					{
						SpanLink outsideTree(other, r);
						bestChildLink = SpanLink::bestSpanLink(bestChildLink, outsideTree);
					}
				}
			}
		}

		return bestChildLink;
	}

	void Clump::setParent(Primitive* p, Primitive* newParent)
	{
		p->setClumpDepth(newParent->getClumpDepth() + 1);

		Joint* j = Primitive::getJoint(p, newParent);
		RigidJoint* r = rbx_static_cast<RigidJoint*>(j);

		p->getBody()->setParent(newParent->getBody());
		p->getBody()->setMeInParent(r->getChildInParent(newParent, p));
	}

	void Clump::setSpanParent(Primitive* onTree, RigidJoint* stop)
	{
		for (RigidJoint* r = onTree->getFirstRigid(); r != NULL; r = onTree->getNextRigid(r))
		{
			if (r != stop)
			{
				Primitive* prim = SpanLink::isSpanningJoint(r);
				if (prim && prim != onTree)
				{
					setSpanParent(prim, stop);
					onTree->getBody()->setParent(NULL);
					setParent(prim, onTree);
				}
			}
		}
	}

	// 100% match if isSpanningJoint has __forceinline
	void Clump::renumberSpanTree(Primitive* onTree)
	{
		for (RigidJoint* r = onTree->getFirstRigid(); r != NULL; r = onTree->getNextRigid(r))
		{
			Primitive* p = SpanLink::isSpanningJoint(r);

			if (p && p == onTree)
			{
				Primitive* other = r->otherPrimitive(onTree);
				other->setClumpDepth(onTree->getClumpDepth() + 1);
				renumberSpanTree(other);
			}
		}
	}

	// 100% match if inSpanTree has __declspec(noinline)
	bool Clump::spanningTreeAdjust(RigidJoint* removing)
	{
		Primitive* prim = SpanLink::isSpanningJoint(removing);
		RBXASSERT(prim);

		Primitive* otherPrim = removing->otherPrimitive(prim);
		RBXASSERT(otherPrim->getClumpDepth() == prim->getClumpDepth() + 1);

		SpanLink bestLink = findBestOutsideLink(otherPrim, otherPrim);
		RigidJoint* joint = bestLink.joint;
		Primitive* parent = bestLink.parent;
		if (parent)
		{
			RBXASSERT(joint != removing);

			Primitive* otherPrimLink = joint->otherPrimitive(parent);
			RBXASSERT(inSpanTree(otherPrimLink, prim));
			RBXASSERT(inSpanTree(otherPrimLink, otherPrim));
			RBXASSERT(!inSpanTree(parent, otherPrim));

			setSpanParent(otherPrimLink, removing);
			setParent(otherPrimLink, parent);
			renumberSpanTree(otherPrimLink);

			RBXASSERT(inSpanTree(otherPrim, otherPrimLink));
			
			return true;
		}

		return false;
	}

	Body* Clump::getRootBody() const
	{
		RBXASSERT(primitives.size() > 0);
		return rootPrimitive->getBody();
	}

	void Clump::putInKernel(Kernel* kernel)
	{
		RBXASSERT(!anchor);
		return kernel->insertBody(getRootBody());
	}

	void Clump::removeFromKernel(Kernel* kernel)
	{
		RBXASSERT(primitives.size() > 0);
		return kernel->removeBody(getRootBody());
	}

	void Clump::setSleepStatus(Sim::AssemblyState _set)
	{
		RBXASSERT(primitives.size() > 0);
		if (_set != sleepStatus)
		{
			if (_set == Sim::AWAKE)
			{
				getRootBody()->resetAccumulators();
			}
			else if (_set == Sim::SLEEPING_CHECKING || _set == Sim::SLEEPING_DEEPLY)
			{
				getRootBody()->setVelocity(Velocity::zero());
			}

			sleepStatus = _set;
		}
	}

	void Clump::addPrimitive(Primitive* p, Primitive* parent, RigidJoint* j)
	{
		RBXASSERT(!p->getAnchorObject());
		RBXASSERT(!p->getClump());
		RBXASSERT(p->getClumpDepth() == -1);

		p->setClump(this);
		setParent(p, parent);
		bool success = primitives.insert(p).second;
		RBXASSERT(success);
		canSleep = canSleep && p->getCanSleep();
	}

	void Clump::onPrimitiveCanSleepChanged(Primitive* p)
	{
		RBXASSERT(containsPrimitive(p));
		bool primCanSleep = p->getCanSleep();
		if (primCanSleep)
		{
			if (!canSleep)
				canSleep = computeCanSleep();
		}
		else
		{
			if (canSleep)
				canSleep = false;
		}
	}

	bool Clump::calcShouldSleep()
	{
		RBXASSERT(primitives.size() > 0);

		G3D::CoordinateFrame cofm = rootPrimitive->getBody()->getBranchCofmCoordinateFrame();
		runningAverageState.update(cofm, MaxRadius);

		return runningAverageState.withinTolerance(cofm, MaxRadius, 0.02f);
	}

	bool Clump::okNeighborSleep()
	{
		RBXASSERT(primitives.size() > 0);

		G3D::CoordinateFrame cofm = rootPrimitive->getBody()->getBranchCofmCoordinateFrame();
		return runningAverageState.withinTolerance(cofm, MaxRadius, 0.02f);
	}

	bool Clump::forceNeighborAwake()
	{
		RBXASSERT(primitives.size() > 0);

		G3D::CoordinateFrame cofm = rootPrimitive->getBody()->getBranchCofmCoordinateFrame();
		return !runningAverageState.withinTolerance(cofm, MaxRadius, 0.04f);
	}

	bool Clump::computeCanSleep()
	{
		typedef std::set<Primitive*>::const_iterator Iterator;

		for (Iterator iter = primitives.begin(); iter != primitives.end(); iter++)
		{
			if (!(*iter)->getCanSleep())
				return false;
		}

		return true;
	}

	void Clump::addInconsistent(RigidJoint* r)
	{
		bool success = inconsistentJoints.insert(r).second;
		RBXASSERT(success);
	}

	bool Clump::containsInconsistent(RigidJoint* r)
	{
		return inconsistentJoints.find(r) != inconsistentJoints.end();
	}

	bool Clump::containsPrimitive(Primitive* p) const
	{
		return primitives.find(p) != primitives.end();
	}

	Clump::~Clump()
	{
		RBXASSERT(!rootPrimitive);
		RBXASSERT(primitives.size() == 0);
		RBXASSERT(inconsistentJoints.size() == 0);
		RBXASSERT(!anchor);
	}

	void Clump::removeInconsistent(RigidJoint* r)
	{
		size_t count = inconsistentJoints.erase(r);
		RBXASSERT(count == 1);
	}

	void Clump::removePrimitive(Primitive* p)
	{
		if (p == rootPrimitive)
			rootPrimitive = NULL;

		MaxRadius.setDirty();
		p->setClump(NULL);
		p->setClumpDepth(-1);
		p->getBody()->setParent(NULL);

		size_t count = primitives.erase(p);
		RBXASSERT(count == 1);

		canSleep = computeCanSleep();
	}

	float Clump::computeMaxRadius() const
	{
		typedef std::set<Primitive*>::const_iterator Iterator;

		if(primitives.size() == 1)
			return (rootPrimitive->getGeometry()->getGridSize() * 0.5f).magnitude();
		else
		{
			float answer = 0.0f;

			RBXASSERT(primitives.size() > 0);

			G3D::Vector3 branchPos = rootPrimitive->getBody()->getBranchCofmPos();

			for(Iterator it = primitives.begin(); it != primitives.end(); it++)
			{
				Primitive* current = *it;

				G3D::Vector3 offsetCenter = Math::vector3Abs(current->getBody()->getPV().position.translation - branchPos);
				G3D::Vector3 offset = offsetCenter + current->getGeometry()->getGridSize() * 0.5f;

				answer = G3D::max(answer, offset.magnitude());
			}

			return answer;
		}
	}

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	Clump::Clump(Primitive* root)
		: sleepStatus(Sim::AWAKE),
		  runningAverageState(),
		  assembly(NULL),
		  rootPrimitive(root),
		  anchor(NULL),
		  canSleep(true),
		  primitives(),
		  inconsistentJoints(),
		  MaxRadius(this, &Clump::computeMaxRadius)
	{
		RBXASSERT(root->getClumpDepth() == -1);
		primitives.insert(root);
		root->setClump(this);
		root->setClumpDepth(0);
	}
#pragma warning (pop)

	void Clump::removeAllPrimitives()
	{
		while (primitives.size() != 0)
		{
			removePrimitive(*primitives.begin());
		}
	}

	// inling problem with the 2nd deque.push_back prevents matching
	void Clump::getRigidJoints(std::set<RigidJoint*>& internalRigids, std::set<RigidJoint*>& externalRigids)
	{
		RBXASSERT(rootPrimitive);

		std::deque<Primitive*> deque;
		deque.push_back(rootPrimitive);

		while (!deque.empty())
		{
			Primitive* p = deque.front();
			deque.pop_front();

			for (RigidJoint* r = p->getFirstRigid(); r != NULL; r = p->getNextRigid(r))
			{
				if (r->getPrimitive(0)->getClump() == this && r->getPrimitive(1)->getClump() == this)
				{
					if (internalRigids.insert(r).second)
					{
						deque.push_back(r->otherPrimitive(p));
					}
				}
				else
				{
					externalRigids.insert(r);
				}
			}
		}
	}
}
