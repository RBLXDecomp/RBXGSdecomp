#include "v8world/ClumpStage.h"
#include "v8world/Clump.h"
#include "v8world/Primitive.h"
#include "v8world/RigidJoint.h"
#include "v8world/MotorJoint.h"
#include "v8world/SleepStage.h"
#include "v8world/CollisionStage.h"
#include "v8world/AssemblyStage.h"
#include "v8world/Assembly.h"
#include "v8world/Anchor.h"

namespace RBX
{
	PrimitiveSort::PrimitiveSort()
		: anchored(false),
		  surfaceAreaJoints(0)
	{
	}

	PrimitiveSort::PrimitiveSort(const Primitive* p)
	{
		float planar = Math::planarSize(p->getGeometry()->getGridSize());
		RBXASSERT(planar < 2147483600.0f);
		RBXASSERT(planar > 0.0f);
		this->surfaceAreaJoints = G3D::iRound(floor(planar)) * p->getNumJoints2();
		this->anchored = p->getAnchor();
	}

	PrimitiveSort::PrimitiveSort(const PrimitiveSort& other)
		: anchored(other.anchored),
		  surfaceAreaJoints(other.surfaceAreaJoints)
	{
	}

	PrimitiveEntry::PrimitiveEntry(Primitive* primitive, PrimitiveSort power)
		: primitive(primitive),
		  power(power)
	{
	}

	AnchorEntry::AnchorEntry(Anchor* anchor, int size)
		: anchor(anchor),
		  size(size)
	{
	}

	RigidEntry::RigidEntry(RigidJoint* rigidJoint, PrimitiveSort power)
		: rigidJoint(rigidJoint),
		  power(power)
	{
	}

	bool PrimitiveSortCriterion::operator()(const PrimitiveEntry& p0, const PrimitiveEntry& p1) const
	{
		if (p0.primitive == p1.primitive)
			return false;

		if (p0.power == p1.power)
			return p0.primitive < p1.primitive;

		return p1.power < p0.power;
	}

	// TODO: NOT CHECKED: this is inlined inside of various std::set functions
	bool AnchorSortCriterion::operator()(const AnchorEntry& a0, const AnchorEntry& a1) const
	{
		if (a0.anchor == a1.anchor)
			return false;

		if (a0.size == a1.size)
			return a0.anchor < a1.anchor;

		return a0.size < a1.size;
	}

	bool RigidSortCriterion::operator()(const RigidEntry& r0, const RigidEntry& r1) const
	{
		if (r0.rigidJoint == r1.rigidJoint)
			return false;

		if (r0.power == r1.power)
			return r0.rigidJoint < r1.rigidJoint;

		return r0.power < r1.power;
	}

	void ClumpStage::anchorsInsert(Anchor* a)
	{
		int planar = G3D::iRound(floor(Math::planarSize(a->getPrimitive()->getGridSize())));

		bool inserted;
		inserted = anchorSizeMap.insert(std::pair<Anchor*, int>(a, planar)).second;
		RBXASSERT(inserted);

		inserted = anchors.insert(AnchorEntry(a, planar)).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::rigidTwosInsert(RigidJoint* r)
	{
		RBXASSERT(!r->getPrimitive(0)->getClump() || !r->getPrimitive(0)->getClump()->containsInconsistent(r));
		RBXASSERT(!r->getPrimitive(1)->getClump() || !r->getPrimitive(1)->getClump()->containsInconsistent(r));
	
		bool inserted = rigidTwos.insert(r).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::rigidOnesInsert(RigidJoint* r)
	{
		PrimitiveSort sort = getRigidPower(r);

		bool inserted;
		inserted = rigidJointPowerMap.insert(std::pair<RigidJoint*, PrimitiveSort>(r, sort)).second;
		RBXASSERT(inserted);

		inserted = rigidOnes.insert(RigidEntry(r, sort)).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::rigidZerosInsert(RigidJoint* r)
	{
		bool inserted = rigidZeros.insert(r).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::primitivesInsert(Primitive* p)
	{
		PrimitiveSort sort(p);

		bool inserted;
		inserted = primitiveSizeMap.insert(std::pair<Primitive*, PrimitiveSort>(p, sort)).second;
		RBXASSERT(inserted);

		inserted = primitives.insert(PrimitiveEntry(p, sort)).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::motorsInsert(MotorJoint* m)
	{
		RBXASSERT(!motorsFind(m));
		motors.push_back(m);
	}

	void ClumpStage::anchoredClumpsInsert(Clump* c)
	{
		bool inserted = anchoredClumps.insert(c).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::freeClumpsInsert(Clump* c)
	{
		bool inserted = freeClumps.insert(c).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::assembliesInsert(Assembly* a)
	{
		bool inserted = assemblies.insert(a).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::edgesInsert(Edge* e)
	{
		bool inserted = edges.insert(e).second;
		RBXASSERT(inserted);
	}

	void ClumpStage::motorAnglesInsert(MotorJoint* m)
	{
		bool inserted = motorAngles.insert(m).second;
		RBXASSERT(inserted);
	}

	bool ClumpStage::anchorsFind(Anchor* a)
	{
		return anchorSizeMap.find(a) != anchorSizeMap.end();
	}

	bool ClumpStage::rigidTwosFind(RigidJoint* r)
	{
		return rigidTwos.find(r) != rigidTwos.end();
	}

	bool ClumpStage::rigidOnesFind(RigidJoint* r)
	{
		return rigidJointPowerMap.find(r) != rigidJointPowerMap.end();
	}

	bool ClumpStage::rigidZerosFind(RigidJoint* r)
	{
		return rigidZeros.find(r) != rigidZeros.end();
	}

	bool ClumpStage::primitivesFind(Primitive* p)
	{
		return primitiveSizeMap.find(p) != primitiveSizeMap.end();
	}

	bool ClumpStage::motorsFind(MotorJoint* m)
	{
		return std::find(motors.begin(), motors.end(), m) != motors.end();
	}

	bool ClumpStage::edgesFind(Edge* e)
	{
		return edges.find(e) != edges.end();
	}

	bool ClumpStage::motorAnglesFind(MotorJoint* m)
	{
		return motorAngles.find(m) != motorAngles.end();
	}

	void ClumpStage::anchorsErase(Anchor* a)
	{
		typedef std::map<Anchor*, int>::iterator Iterator;

		Iterator it = anchorSizeMap.find(a);

		std::pair<Anchor*, int> pair = *it;
		AnchorEntry entry(a, pair.second); 

		size_t removed = anchors.erase(entry);
		RBXASSERT(removed == 1);

		anchorSizeMap.erase(it);
	}

	void ClumpStage::rigidTwosErase(RigidJoint* r)
	{
		size_t removed = rigidTwos.erase(r);
		RBXASSERT(removed == 1);
	}

	void ClumpStage::rigidOnesErase(RigidJoint* r)
	{
		typedef std::map<RigidJoint*, PrimitiveSort>::iterator Iterator;

		Iterator it = rigidJointPowerMap.find(r);

		std::pair<RigidJoint*, PrimitiveSort> pair = *it;
		RigidEntry entry(r, pair.second); 

		size_t removed = rigidOnes.erase(entry);
		RBXASSERT(removed == 1);

		rigidJointPowerMap.erase(it);
	}

	void ClumpStage::rigidZerosErase(RigidJoint* r)
	{
		size_t removed = rigidZeros.erase(r);
		RBXASSERT(removed == 1);
	}

	void ClumpStage::primitivesErase(Primitive* p)
	{
		typedef std::map<Primitive*, PrimitiveSort>::iterator Iterator;

		Iterator it = primitiveSizeMap.find(p);

		std::pair<Primitive*, PrimitiveSort> pair = *it;
		PrimitiveEntry entry(p, pair.second); 

		size_t removed = primitives.erase(entry);
		RBXASSERT(removed == 1);

		primitiveSizeMap.erase(it);
	}

	void ClumpStage::motorsErase(MotorJoint* m)
	{
		RBXASSERT(motorsFind(m));
		motors.erase(std::find(motors.begin(), motors.end(), m));
	}

	void ClumpStage::anchoredClumpsErase(Clump* c)
	{
		size_t removed = anchoredClumps.erase(c);
		RBXASSERT(removed == 1);
	}

	void ClumpStage::freeClumpsErase(Clump* c)
	{
		size_t removed = freeClumps.erase(c);
		RBXASSERT(removed == 1);
	}

	void ClumpStage::motorAnglesErase(MotorJoint* m)
	{
		size_t removed = motorAngles.erase(m);
		RBXASSERT(removed == 1);
	}

	bool lessClump(const Clump& c0, const Clump& c1)
	{
		return PrimitiveSort(c0.getRootPrimitive()) < PrimitiveSort(c1.getRootPrimitive());
	}

	bool lessMotor(const MotorJoint* m0, const MotorJoint* m1)
	{
		if (m0 == m1)
			return false;

		PrimitiveSort power0 = ClumpStage::getMotorPower(m0);
		PrimitiveSort power1 = ClumpStage::getMotorPower(m1);

		if (power0.anchored != power1.anchored)
			return power1.anchored;

		if (power0.surfaceAreaJoints == power1.surfaceAreaJoints)
			return m0 < m1;

		return power0.surfaceAreaJoints < power1.surfaceAreaJoints;
	}

	PrimitiveSort ClumpStage::getMotorPower(const MotorJoint* m)
	{
		Clump* clump0 = m->getPrimitive(0)->getClump();
		Clump* clump1 = m->getPrimitive(1)->getClump();
		RBXASSERT(clump0 && clump1);

		if (clump0 == clump1)
		{
			return PrimitiveSort();
		}
		else
		{
			Primitive* c1Root = clump1->getRootPrimitive();
			Primitive* c0Root = clump0->getRootPrimitive();

			return G3D::max(PrimitiveSort(c0Root), PrimitiveSort(c1Root));
		}
	}

	PrimitiveSort ClumpStage::getRigidPower(RigidJoint* r)
	{
		Clump* clump0 = r->getPrimitive(0)->getClump();
		Clump* clump1 = r->getPrimitive(1)->getClump();
		RBXASSERT((clump0 == NULL) != (clump1 == NULL));
		if (!clump0)
			clump0 = clump1;
		RBXASSERT(clump0);
		return PrimitiveSort(clump0->getRootPrimitive());
	}

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	ClumpStage::ClumpStage(IStage* upstream, World* world)
		: IWorldStage(upstream, new AssemblyStage(this, world), world)
	{
	}
#pragma warning (pop)

	ClumpStage::~ClumpStage()
	{
	}

	void ClumpStage::stepUi(int uiStepId)
	{
		RBXASSERT(upToDate());
		rbx_static_cast<AssemblyStage*>(getDownstreamWS())->stepUi(uiStepId);
	}

	void ClumpStage::stepWorld(int worldStepId, int uiStepId, bool throttling)
	{
		RBXASSERT(upToDate());
		process();

		AssemblyStage* assemblyStage = rbx_static_cast<AssemblyStage*>(getDownstreamWS());
		assemblyStage->stepWorld(worldStepId, uiStepId, throttling);
	}

	bool ClumpStage::upToDate()
	{
		return 
			anchors.empty() &&
			rigidTwos.empty() &&
			rigidOnes.empty() &&
			rigidZeros.empty() &&
			primitives.empty() &&
			motors.empty() &&
			anchoredClumps.empty() &&
			freeClumps.empty() &&
			assemblies.empty();
	}

	void ClumpStage::process()
	{
		do
		{
			do
			{
				do
				{
					processAnchors();
				}
				while (!processRigidTwos());
			}
			while (!processRigidOnes());

			RBXASSERT(anchors.empty());
			RBXASSERT(rigidTwos.empty());
			RBXASSERT(rigidOnes.empty());
		}
		while (!processPrimitives());
		processMotors();
		processAssemblies();
		processEdges();
		processMotorAngles();
	}

	void ClumpStage::processAnchors()
	{
		typedef std::set<AnchorEntry, AnchorSortCriterion>::const_iterator Iterator;

		while (!anchors.empty())
		{
			Iterator it = anchors.end();
			it--;

			Anchor* a = (*it).anchor;
			Primitive* p = a->getPrimitive();
			bool hasClump = p->getClump() != NULL;

			RBXASSERT(hasClump != primitivesFind(p));

			if (hasClump)
			{
				Clump* c = p->getClump();
				if (p != c->getRootPrimitive() || c->getAssembly())
				{
					destroyClump(c);
				}
				else
				{
					anchorsErase(a);
					c->addAnchor(a);
				}
			}
			else
			{
				Clump* c = new Clump(p);
				c->addAnchor(a);
				anchoredClumpsInsert(c);
				anchorsErase(a);
				primitivesErase(p);

				for (RigidJoint* r = p->getFirstRigid(); r != NULL; r = p->getNextRigid(r))
				{
					RBXASSERT(inBuffers(r));
					removeFromBuffers(r);

					if (c->otherClump(r))
					{
						//rigidTwosInsert(r);
						rigidTwos.insert(r);
					}
					else
					{
						rigidOnesInsert(r);
					}
				}
			}
		}
	}

	// TODO: match this better... this function sucks
	bool ClumpStage::processRigidTwos()
	{
		while (!rigidTwos.empty())
		{
			RBXASSERT(!anchors.empty());
			RigidJoint* r = *rigidTwos.begin();

			Clump* c0 = r->getPrimitive(0)->getClump();
			Clump* c1 = r->getPrimitive(1)->getClump();

			RBXASSERT(!c0 || !c0->containsInconsistent(r));
			RBXASSERT(!c1 || !c1->containsInconsistent(r));

			if (c0 && c0->getAssembly())
				destroyAssembly(c0->getAssembly());
			if (c1 && c1->getAssembly())
				destroyAssembly(c1->getAssembly());

			RBXASSERT(!c0 || !c0->containsInconsistent(r));
			RBXASSERT(!c1 || !c1->containsInconsistent(r));

			if (c0 || c1)
			{
				if ((c0 && !c1) || (!c0 && c1))
				{
					rigidOnesInsert(r);
					rigidTwosErase(r);
					//RBXASSERT(!anchors.empty());
				}
				else if (c0 == c1)
				{
					rigidTwosErase(r);	
					//RBXASSERT(!anchors.empty());
				}
				else
				{
					Clump* bc;
					if (c0->getAnchored())
						bc = c1;
					else if (c1->getAnchored())
						bc = c0;
					else if (lessClump(*c0, *c1))
						bc = c0;
					else
						bc = c1;

					if (!bc->getAnchored())
					{
						destroyClump(bc);
						RBXASSERT(!anchors.empty());
					}
					else
					{
						RBXASSERT(!c0->containsInconsistent(r));
						RBXASSERT(!c1->containsInconsistent(r));

						PrimitiveSort power0(c0->getRootPrimitive());
						PrimitiveSort power1(c1->getRootPrimitive());

						if (power0 == power1)
						{
							rigidTwosErase(r);
							c0->addInconsistent(r);
							c1->addInconsistent(r);
						}
						else
						{
							bool bruh = power0 < power1;
							Clump* bruhClump = bruh ? c0 : c1;

							if (bruhClump->size() > 1)
							{
								destroyClump(bruhClump);
								RBXASSERT(!rigidOnesFind(r));

								return false;
							}
							else
							{
								rigidTwosErase(r);
								c0->addInconsistent(r);
								c1->addInconsistent(r);
							}
						}
					}
				}

				RBXASSERT(!anchors.empty());
			}
			else
			{
				rigidZerosInsert(r);
				rigidTwosErase(r);
			}
		}

		return true;
	}

	// 30% match if numClumps has __declspec(noinline)
	// difficult to match as the std::set::erase inside of rigidZerosErase isnt inlining right
	bool ClumpStage::processRigidOnes()
	{
		typedef std::set<RigidEntry, RigidSortCriterion>::const_iterator Iterator;

		while (!rigidOnes.empty())
		{
			Iterator it = rigidOnes.end();
			it--;

			RigidJoint* r = (*it).rigidJoint;
			Clump* c0 = r->getPrimitive(0)->getClump();
			Clump* c1 = r->getPrimitive(1)->getClump();

			RBXASSERT(numClumps(r) == 1);

			Clump* c;
			if (c0)
			{
				c = c0;
			}
			else
			{
				c = c1;
				c0 = c1;
			}

			if (c0->getAssembly())
				destroyAssembly(c0->getAssembly());

			Primitive* p0 = r->getPrimitive(0);
			Primitive* p1;
			if (r->getPrimitive(0)->getClump() == c0)
			{
				p1 = r->getPrimitive(1);
			}
			else
			{
				p1 = r->getPrimitive(0);
			}

			Primitive* base = (p1 == p0) ? r->getPrimitive(1) : r->getPrimitive(0);

			primitivesErase(p1);

			if (PrimitiveSort(c0->getRootPrimitive()) < PrimitiveSort(p1))
			{
				primitivesInsert(p1);
				destroyClump(c);
				return false;
			}

			c->addPrimitive(p1, base, r);
			RigidJoint* p1R = p1->getFirstRigid();
			bool ok = true;
			while (p1R != NULL)
			{
				Primitive* other = p1R->otherPrimitive(p1);
				if (other == base)
				{
					rigidOnesErase(p1R);
					RBXASSERT(p1R == r);
				}
				else if (other->getClump() == NULL)
				{
					rigidZerosErase(p1R);
					rigidOnesInsert(p1R);
				}
				else if (other->getClump() == c)
				{
					rigidOnesErase(p1R);
				}
				else
				{
					rigidOnesErase(p1R);
					rigidTwosInsert(p1R);
					ok = false;
				}
				p1R = p1->getNextRigid(p1R);
			}
			if (!ok)
				return false;
		}

		return true;
	}

	bool ClumpStage::processPrimitives()
	{
		while (!primitives.empty())
		{
			std::set<PrimitiveEntry, PrimitiveSortCriterion>::iterator it = --primitives.end();
			Primitive* p = (*it).primitive;
			primitivesErase(p);

			RBXASSERT(!p->getClump());
			Clump* c = new Clump(p);
			freeClumps.insert(c);

			RigidJoint* r = p->getFirstRigid();
			if (r)
			{
				// TODO: VERY UGLY - forces everything in this block to inline. 
				// is there a better way of doing this?
				SCOPED(SCOPED(
					do
					{
						rigidZerosErase(r);
						rigidOnesInsert(r);
					}
					while (r = p->getNextRigid(r));
				););

				return false;
			}
		}

		RBXASSERT(rigidZeros.empty());
		return true;
	}

	void ClumpStage::processMotors()
	{
		while (!anchoredClumps.empty())
		{
			Clump* c = *anchoredClumps.begin();
			anchoredClumpsErase(c);

			Assembly* a = new Assembly(c);
			assembliesInsert(a);
		}

		typedef std::vector<MotorJoint*>::const_iterator MotorIterator;
		std::sort(motors.begin(), motors.end(), lessMotor);
		std::set<Assembly*> assembliesToDestroy;

		for (MotorIterator it = motors.begin(); it != motors.end(); it++)
		{
			MotorJoint* m = *it;
			for (int i = 0; i < 2; i++)
			{
				Primitive* p = m->getPrimitive(i);
				RBXASSERT(p);
				RBXASSERT(p->inStage(this));

				Assembly* a = p->getAssembly();
				if (a && !a->getRootClump()->getAnchored())
				{
					assembliesToDestroy.insert(a);
				}
			}
		}

		typedef std::set<Assembly*>::const_iterator AssemblyIterator;

		for (AssemblyIterator aIt = assembliesToDestroy.begin(); aIt != assembliesToDestroy.end(); aIt++)
		{
			destroyAssembly(*aIt);
		}

		while (!motors.empty())
		{
			MotorJoint* m = motors.back();
			motors.pop_back();

			Assembly* a0 = m->getPrimitive(0)->getAssembly();
			Assembly* a1 = m->getPrimitive(1)->getAssembly();

			Clump* c0 = m->getPrimitive(0)->getClump();
			Clump* c1 = m->getPrimitive(1)->getClump();

			if (a0)
			{
				if (a1)
				{
					a0->addInconsistentMotor(m);
					if (a0 != a1)
						a1->addInconsistentMotor(m);
				}
				else
				{
					freeClumpsErase(c1);
					a0->addClump(c1, m);
				}
			}
			else if (a1)
			{
				freeClumpsErase(c0);
				a1->addClump(c0, m);
			}
			else if (c0 == c1)
			{
				freeClumpsErase(c0);
				Assembly* a = new Assembly(c0);
				a->addInconsistentMotor(m);
				assembliesInsert(a);
			}
			else
			{
				const Primitive* p1 = c1->getRootPrimitive();
				const Primitive* p0 = c0->getRootPrimitive();

				Clump* root;
				Clump* child;

				if (PrimitiveSort(p0) < PrimitiveSort(p1))
				{
					root = c1;
					child = c0;
				}
				else
				{
					root = c0;
					child = c1;
				}

				freeClumpsErase(root);
				freeClumpsErase(child);
				Assembly* a = new Assembly(root);
				a->addClump(child, m);
				assembliesInsert(a);
			}
		}

		while (!freeClumps.empty())
		{
			Clump* c = *freeClumps.begin();
			freeClumpsErase(c);
			Assembly* a = new Assembly(c);
			assembliesInsert(a);
		}
	}

	void ClumpStage::processAssemblies()
	{
		while (!assemblies.empty())
		{
			Assembly* a = *assemblies.begin();
			size_t removed = assemblies.erase(a);
			RBXASSERT(removed == 1);
			a->putInPipeline(this);
			rbx_static_cast<AssemblyStage*>(getDownstreamWS())->onAssemblyAdded(a);
		}
	}

	void ClumpStage::processEdges()
	{
		while (!edges.empty())
		{
			Edge* e = *edges.begin();
			edges.erase(edges.begin());

			RBXASSERT(e->inStage(this));

			Assembly* a0 = e->getPrimitive(0)->getAssembly();
			Assembly* a1 = e->getPrimitive(1)->getAssembly();

			RBXASSERT(a0 && a1);
			RBXASSERT(a0->downstreamOfStage(this) && a1->downstreamOfStage(this));

			if (a0 == a1)
			{
				a0->addInternalEdge(e);
			}
			else
			{
				a0->addExternalEdge(e);
				a1->addExternalEdge(e);

				if (!a0->getAnchored() || !a1->getAnchored())
				{
					getDownstreamWS()->onEdgeAdded(e);
				}
			}
		}
	}

	void ClumpStage::processMotorAngles()
	{
		while (!motorAngles.empty())
		{
			MotorJoint* j = *motorAngles.begin();
			updateMotorJoint(j);
			size_t removed = motorAngles.erase(j);
			RBXASSERT(removed == 1);
		}
	}

	void ClumpStage::addMotor(MotorJoint* m)
	{
		motorsInsert(m);
		motorAnglesInsert(m);
	}

	void ClumpStage::addEdge(Edge* e)
	{
		edgesInsert(e);
	}

	void ClumpStage::removeRigid(RigidJoint* r)
	{
		if (removeFromBuffers(r))
			return;

		Clump* c = r->getPrimitive(0)->getClump();

		if (c && c == r->getPrimitive(1)->getClump())
		{
			removeFromClump(c, r);
		}
		else
		{
			Clump* c0 = r->getPrimitive(0)->getClump();
			if (c0)
				destroyClump(c0);
			
			Clump* c1 = r->getPrimitive(1)->getClump();
			if (c1)
				destroyClump(c1);
		}

		bool removed = removeFromBuffers(r);
		RBXASSERT(removed);
	}

	void ClumpStage::removeMotor(MotorJoint* m)
	{
		if (!motorsFind(m))
		{
			Assembly* a = m->getPrimitive(0)->getAssembly();
			RBXASSERT(a == m->getPrimitive(1)->getAssembly());
			destroyAssembly(a);
		}

		motorsErase(m);
		if (motorAnglesFind(m))
			motorAnglesErase(m);
	}

	void ClumpStage::removeEdge(Edge* e)
	{
		Assembly* a0 = e->getPrimitive(0)->getAssembly();
		Assembly* a1 = e->getPrimitive(1)->getAssembly();

		if (e->downstreamOfStage(this))
		{
			getDownstreamWS()->onEdgeRemoving(e);

			RBXASSERT(a0 && a1 && a0 != a1);
			a0->removeExternalEdge(e);
			a1->removeExternalEdge(e);

			RBXASSERT(edges.find(e) == edges.end());
		}
		else
		{
			size_t removed = edges.erase(e);
			if (removed != 1)
			{
				RBXASSERT(a0 && a1);
				if (a0 == a1)
				{
					a0->removeInternalEdge(e);
				}
				else
				{
					RBXASSERT(a0->getAnchored() && a1->getAnchored());

					a0->removeExternalEdge(e);
					a1->removeExternalEdge(e);
				}
			}
		}

		RBXASSERT(e->inStage(this));
	}

	// 100% match if primitivesFind has __forceinline
	void ClumpStage::removeAnchor(Anchor* a)
	{
		if (!anchorsFind(a))
		{
			if (a->getPrimitive()->getClump())
			{
				destroyClump(a->getPrimitive()->getClump());
			}
			else
			{
				RBXASSERT(primitivesFind(a->getPrimitive()));
			}
		}

		anchorsErase(a);
	}

	void ClumpStage::removePrimitive(Primitive* p)
	{
		if (!primitivesFind(p))
		{
			if (p->getClump())
				destroyClump(p->getClump());
		}

		primitivesErase(p);
	}

	void ClumpStage::destroyClump(Clump* c)
	{
		if (c->getAssembly())
			destroyAssembly(c->getAssembly());

		if (c->getAnchored())
			anchoredClumpsErase(c);
		else
			freeClumpsErase(c);

		destroyClumpGuts(c);
		delete c;
	}

	void ClumpStage::destroyClumpGuts(Clump* c)
	{
		typedef std::set<Primitive*>::const_iterator PrimIterator;

		for (PrimIterator it = c->clumpPrimBegin(); it != c->clumpPrimEnd(); it++)
		{
			Primitive* p = *it;
			primitivesInsert(p);
		}

		if (c->getAnchored())
		{
			Anchor* anchor = c->removeAnchor();
			anchorsInsert(anchor);
		}

		while (!c->getInconsistents().empty())
		{
			RigidJoint* r = *c->getInconsistents().begin();

			Clump* otherClump = c->otherClump(r);
			RBXASSERT(otherClump != c);

			otherClump->removeInconsistent(r);
			c->removeInconsistent(r);
			rigidTwosInsert(r);
		}

		std::set<RigidJoint*> internalRs;
		std::set<RigidJoint*> externalRs;
		c->getRigidJoints(internalRs, externalRs);

		typedef std::set<RigidJoint*>::const_iterator RigidIterator;

		for (RigidIterator it = internalRs.begin(); it != internalRs.end(); it++)
		{
			RigidJoint* r = *it;
			RBXASSERT(r->getPrimitive(0)->getClump() == r->getPrimitive(1)->getClump());
			if (!inBuffers(r))
				rigidTwosInsert(r);
		}

		c->removeAllPrimitives();

		for (RigidIterator it = externalRs.begin(); it != externalRs.end(); it++)
		{
			RigidJoint* r = *it;

			size_t removed = rigidTwos.erase(r);
			if (removed == 0)
			{
				removed = rigidZeros.erase(r);
				if (removed == 0)
				{
					if (rigidOnesFind(r))
						rigidOnesErase(r);
				}
			}

			RBXASSERT(numClumps(r) < 2);
			int count = numClumps(r);
			if (count != 1)
				rigidOnesInsert(r);
			else
				rigidZerosInsert(r);
		}
	}

	void ClumpStage::destroyAssembly(Assembly* a)
	{
		size_t removed = assemblies.erase(a);
		if (removed == 0)
		{
			RBXASSERT(a->downstreamOfStage(this));
			removeAssemblyEdges(a);

			AssemblyStage* assemblyStage = rbx_static_cast<AssemblyStage*>(getDownstreamWS());
			assemblyStage->onAssemblyRemoving(a);
		}
		else
		{
			RBXASSERT(a->inStage(this));
		}

		std::vector<MotorJoint*>& motors = a->getMotors();
		while (!motors.empty())
		{
			MotorJoint* m = motors.front();
			a->removeMotor(m);

			motorsInsert(m);
			if (!motorAnglesFind(m))
				motorAnglesInsert(m);
		}

		std::set<MotorJoint*>& inconsistentMotors = a->getInconsistentMotors();
		while (!inconsistentMotors.empty())
		{
			MotorJoint* m = *inconsistentMotors.begin();

			a->removeInconsistentMotor(m);
			Assembly* a1 = a->otherAssembly(m);
			if (a1 != a)
				a1->removeInconsistentMotor(m);

			motorsInsert(m);
		}

		std::set<Clump*>& clumps = a->getClumps();
		while (!clumps.empty())
		{
			Clump* c = *clumps.begin();
			a->removeClump(c);

			if (c->getAnchored())
				anchoredClumpsInsert(c);
			else
				freeClumpsInsert(c);
		}

		RBXASSERT(a->inStage(this));
		a->removeFromPipeline(this);
		delete a;
	}

	// 100% match if downstreamOfStage in RBXASSERT has __forceinline
	void ClumpStage::removeExternalEdge(Edge* e)
	{
		Assembly* a0 = e->getPrimitive(0)->getAssembly();
		Assembly* a1 = e->getPrimitive(1)->getAssembly();

		RBXASSERT(e->downstreamOfStage(this) != (a0->getAnchored() && a1->getAnchored()));
		RBXASSERT(!edgesFind(e));

		a0->removeExternalEdge(e);
		a1->removeExternalEdge(e);
		if (e->downstreamOfStage(this))
			getDownstreamWS()->onEdgeRemoving(e);

		edgesInsert(e);
	}

	void ClumpStage::removeInternalEdge(Edge* e)
	{
		RBXASSERT(e->inStage(this));
		RBXASSERT(!edgesFind(e));
		RBXASSERT(e->getPrimitive(0)->getAssembly() == e->getPrimitive(1)->getAssembly());

		e->getPrimitive(0)->getAssembly()->removeInternalEdge(e);
		edgesInsert(e);
	}

	void ClumpStage::removeAssemblyEdges(Assembly* a)
	{
		while (!a->getExternalEdges().empty())
		{
			removeExternalEdge(*a->getExternalEdges().begin());
		}

		while (!a->getInternalEdges().empty())
		{
			removeInternalEdge(*a->getInternalEdges().begin());
		}
	}

	void ClumpStage::onPrimitiveAdded(Primitive* p)
	{
		RBXASSERT(!p->getFirstEdge());
		p->putInStage(this);
		primitivesInsert(p);
		if (p->getAnchorObject())
			anchorsInsert(p->getAnchorObject());
	}

	void ClumpStage::onPrimitiveRemoving(Primitive* p)
	{
		if (p->getAnchorObject())
			removeAnchor(p->getAnchorObject());
		removePrimitive(p);
		p->removeFromStage(this);
	}

	void ClumpStage::onPrimitiveAddedAnchor(Primitive* p)
	{
		anchorsInsert(p->getAnchorObject());
	}

	void ClumpStage::onPrimitiveRemovingAnchor(Primitive* p)
	{
		removeAnchor(p->getAnchorObject());
	}

	void ClumpStage::onEdgeAdded(Edge* e)
	{
		RBXASSERT(e->getPrimitive(0)->inOrDownstreamOfStage(this));
		RBXASSERT(e->getPrimitive(1)->inOrDownstreamOfStage(this));

		Primitive::insertEdge(e);
		e->putInStage(this);
		if (RigidJoint::isRigidJoint(e))
		{
			rigidTwosInsert(rbx_static_cast<RigidJoint*>(e));
		}
		else if (MotorJoint::isMotorJoint(e))
		{
			addMotor(rbx_static_cast<MotorJoint*>(e));
		}
		else
		{
			addEdge(e);
		}
	}

	void ClumpStage::onEdgeRemoving(Edge* e)
	{
		RBXASSERT(e->getPrimitive(0)->inOrDownstreamOfStage(this));
		RBXASSERT(e->getPrimitive(1)->inOrDownstreamOfStage(this));

		if (RigidJoint::isRigidJoint(e))
		{
			removeRigid(rbx_static_cast<RigidJoint*>(e));
		}
		else if (MotorJoint::isMotorJoint(e))
		{
			removeMotor(rbx_static_cast<MotorJoint*>(e));
		}
		else
		{
			removeEdge(e);
		}
		e->removeFromStage(this);
		Primitive::removeEdge(e);
	}

	void ClumpStage::onPrimitiveCanSleepChanged(Primitive* p)
	{
		process();

		Assembly* a = p->getAssembly();
		RBXASSERT(a);

		bool canSleep = a->getCanSleep();
		a->onPrimitiveCanSleepChanged(p);

		if (canSleep != a->getCanSleep() && a->getSleepStatus() != Sim::AWAKE)
		{
			IStage* stage = findStage(SLEEP_STAGE);
			SleepStage* sleepStage = rbx_static_cast<SleepStage*>(stage);
			sleepStage->onWakeUpRequest(a);
		}
	}

	void ClumpStage::onPrimitiveCanCollideChanged(Primitive* p)
	{
		IStage* collisionIStage = findStage(COLLISION_STAGE);
		rbx_static_cast<CollisionStage*>(collisionIStage)->onPrimitiveCanCollideChanged(p);
	}

	void ClumpStage::onMotorAngleChanged(MotorJoint* m)
	{
		RBXASSERT(motorsFind(m) || (m->getPrimitive(0)->getAssembly() && m->getPrimitive(1)->getAssembly()));

		if (!motorAnglesFind(m))
			motorAnglesInsert(m);
	}

	void ClumpStage::updateMotorJoint(MotorJoint* m)
	{
		RBXASSERT(m->getPrimitive(0)->getAssembly());
		RBXASSERT(m->getPrimitive(1)->getAssembly());
		RBXASSERT(!motorsFind(m));

		Primitive* p0 = m->getPrimitive(0);
		Primitive* p1 = !p0->getClump()->getRootPrimitive()->getBody()->getParent() ? m->getPrimitive(1) : p0;
		if (p1 == p0)
			p0 = m->getPrimitive(1);

		Primitive* p1Root = p1->getClump()->getRootPrimitive();
		RBXASSERT(p1Root->getBody()->getParent() == p0->getBody());

		G3D::CoordinateFrame mChildInMParent = m->getMeInOther(p1);
		if (p1 == p1Root)
		{
			p1Root->getBody()->setMeInParent(mChildInMParent);
		}
		else
		{
			G3D::CoordinateFrame mChildInCRoot = p1->getBody()->getMeInDescendant(p1Root->getBody());
			G3D::CoordinateFrame cRootInMChild = mChildInCRoot.inverse();
			G3D::CoordinateFrame cRootInMParent = mChildInMParent * cRootInMChild;
			p1Root->getBody()->setMeInParent(cRootInMParent);
		}
	}

	void ClumpStage::removeFromClump(Clump* c, RigidJoint* r)
	{
		Primitive* p0 = SpanLink::isSpanningJoint(r);

		if (p0)
		{
			Primitive* p1 = (p0 == r->getPrimitive(0) ? r->getPrimitive(1) : r->getPrimitive(0)); // NOTE: not r->otherPrimitive()
			if (!p0->getClump()->spanningTreeAdjust(r))
			{
				removeSpanningTreeFast(p1, r);
				return;
			}
		}

		rigidTwosInsert(r);
	}

	void ClumpStage::removeFromAssemblyFast(Primitive* p)
	{
		Assembly* a = p->getAssembly();
		RBXASSERT(a);

		Edge* e = p->getFirstEdge();

		if (e)
		{
			for (; e != NULL; e = p->getNextEdge(e))
			{
				SCOPED( // TODO: see ClumpStage::processPrimitives
				if (MotorJoint::isMotorJoint(e))
				{
					destroyAssembly(p->getAssembly());
					return;
				}


				if (!RigidJoint::isRigidJoint(e))
				{
					if (a->containsExternalEdge(e))
						removeExternalEdge(e);
					else if (a->containsInternalEdge(e))
						removeInternalEdge(e);
				}
				);
			}
		}
	}

	void ClumpStage::removeFromClumpFast(Primitive* p, RigidJoint* toParent)
	{
		if (p->getAssembly())
			removeFromAssemblyFast(p);

		Clump* c = p->getClump();
		
		RigidJoint* r = p->getFirstRigid();
		while (r != NULL)
		{
			if (c->containsInconsistent(r))
			{
				RBXASSERT(!inBuffers(r));
				c->removeInconsistent(r);
				c->otherClump(r)->removeInconsistent(r);
			}
			else if (inBuffers(r))
			{
				removeFromBuffers(r);
			}

			rigidTwosInsert(r);
			r = p->getNextRigid(r);
		}

		primitivesInsert(p);
		c->removePrimitive(p);
	}

	void ClumpStage::removeSpanningTreeFast(Primitive* topPrim, RigidJoint* toParent)
	{
		for (RigidJoint* r = topPrim->getFirstRigid(); r != NULL; r = topPrim->getNextRigid(r))
		{
			if (SpanLink::isDownstreamSpanningJoint(topPrim, r))
			{
				removeSpanningTreeFast(r->otherPrimitive(topPrim), r);
			}
		}

		removeFromClumpFast(topPrim, toParent);
	}

	int ClumpStage::numClumps(RigidJoint* r)
	{
		int count = 0;
		if (r->getPrimitive(0)->getClump())
			++count;
		if (r->getPrimitive(1)->getClump())
			++count;
		return count;
	}

	bool ClumpStage::inBuffers(RigidJoint* r)
	{
		return rigidTwosFind(r) || rigidZerosFind(r) || rigidOnesFind(r);
	}

	int ClumpStage::getMetric(MetricType metricType)
	{
		if (metricType != MAX_TREE_DEPTH)
			return IWorldStage::getMetric(metricType);

		return 0;
	}

	bool ClumpStage::removeFromBuffers(RigidJoint* r)
	{
		if (rigidTwos.erase(r) != 0 || rigidZeros.erase(r) != 0)
			return true;

		if (rigidOnesFind(r))
		{
			rigidOnesErase(r);
			return true;
		}

		return false;
	}
}
