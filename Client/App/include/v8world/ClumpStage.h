#pragma once
#include <map>
#include <set>
#include <vector>
#include "v8world/IWorldStage.h"

namespace RBX
{
	class Anchor;
	class Primitive;
	class RigidJoint;
	class MotorJoint;
	class Clump;
	class Assembly;

	// MYSTERY CLASS: no size, only referenced here
	class Influence
	{
	};

	class PrimitiveSort
	{
	public:
		bool anchored;
		size_t surfaceAreaJoints;
	  
	public:
		PrimitiveSort(const PrimitiveSort& other);
		PrimitiveSort(const Primitive* p);
		PrimitiveSort();
	public:
		bool operator==(const PrimitiveSort& other) const
		{
			return isEqual(*this, other);
		}
		bool operator!=(const PrimitiveSort& other) const
		{
			return !isEqual(*this, other);
		}
		bool operator<(const PrimitiveSort& other) const
		{
			return isLess(*this, other);
		}
		bool operator>(const PrimitiveSort& other) const
		{
			return isLess(other, *this);
		}
	  
	private:
		static bool isEqual(const PrimitiveSort& s0, const PrimitiveSort& s1)
		{
			return s0.anchored == s1.anchored && s0.surfaceAreaJoints == s1.surfaceAreaJoints;
		}
		static bool isLess(const PrimitiveSort& s0, const PrimitiveSort& s1)
		{
			return s0.anchored != s1.anchored ? s1.anchored : (s0.surfaceAreaJoints < s1.surfaceAreaJoints);
		}
	};

	class PrimitiveEntry
	{
	public:
		Primitive* primitive;
		PrimitiveSort power;

	public:
		PrimitiveEntry(Primitive* primitive, PrimitiveSort power);
	};

	class AnchorEntry
	{
	public:
		Anchor* anchor;
		int size;

	public:
		AnchorEntry(Anchor* anchor, int size);
	};

	class RigidEntry
	{
	public:
		RigidJoint* rigidJoint;
		PrimitiveSort power;

	public:
		RigidEntry(RigidJoint* rigidJoint, PrimitiveSort power);
	};

	class PrimitiveSortCriterion
	{
	public:
		bool operator()(const PrimitiveEntry& p0, const PrimitiveEntry& p1) const;
	};

	class AnchorSortCriterion
	{
	public:
		bool operator()(const AnchorEntry& a0, const AnchorEntry& a1) const;
	};

	class RigidSortCriterion
	{
	public:
		bool operator()(const RigidEntry& r0, const RigidEntry& r1) const;
	};

	class ClumpStage : public IWorldStage
	{
	private:
		World* world;
		std::map<Anchor*, int> anchorSizeMap;
		std::map<Primitive*, PrimitiveSort> primitiveSizeMap;
		std::map<RigidJoint*, PrimitiveSort> rigidJointPowerMap;
		std::set<AnchorEntry, AnchorSortCriterion> anchors;
		std::set<RigidJoint*> rigidTwos;
		std::set<RigidEntry, RigidSortCriterion> rigidOnes;
		std::set<RigidJoint*> rigidZeros;
		std::set<PrimitiveEntry, PrimitiveSortCriterion> primitives;
		std::vector<MotorJoint*> motors;
		std::set<Clump*> anchoredClumps;
		std::set<Clump*> freeClumps;
		std::set<Assembly*> assemblies;
		std::set<Edge*> edges;
		std::set<MotorJoint*> motorAngles;
  
	private:
		Anchor* getBiggestAnchor();
		RigidJoint* getBiggestRigidOne();
		Primitive* getBiggestPrimitive();

		void anchorsInsert(Anchor* a);
		void rigidTwosInsert(RigidJoint* r);
		void rigidOnesInsert(RigidJoint* r);
		void rigidZerosInsert(RigidJoint* r);
		void primitivesInsert(Primitive* p);
		void motorsInsert(MotorJoint* m);
		void anchoredClumpsInsert(Clump* c);
		void freeClumpsInsert(Clump* c);
		void assembliesInsert(Assembly* a);
		void edgesInsert(Edge* e);
		void motorAnglesInsert(MotorJoint* m);

		bool anchorsFind(Anchor* a);
		bool rigidTwosFind(RigidJoint* r);
		bool rigidOnesFind(RigidJoint* r);
		bool rigidZerosFind(RigidJoint* r);
		bool primitivesFind(Primitive* p);
		bool motorsFind(MotorJoint* m);
		bool anchoredClumpsFind(Clump* c);
		bool freeClumpsFind(Clump* c);
		bool assembliesFind(Assembly* a);
		bool edgesFind(Edge* e);
		bool motorAnglesFind(MotorJoint* m);

		void anchorsErase(Anchor* a);
		void rigidTwosErase(RigidJoint* r);
		void rigidOnesErase(RigidJoint* r);
		void rigidZerosErase(RigidJoint* r);
		void primitivesErase(Primitive* p);
		void motorsErase(MotorJoint* m);
		void anchoredClumpsErase(Clump* c);
		void freeClumpsErase(Clump* c);
		void assembliesErase(Assembly* a);
		void edgesErase(Edge* e);
		void motorAnglesErase(MotorJoint* m);

		void processAnchors();
		bool processRigidTwos();
		bool processRigidOnes();
		bool processRigidZeros();
		bool processPrimitives();
		void processMotors();
		void processAssemblies();
		void processEdges();
		void processMotorAngles();

		void addRigid(RigidJoint*);
		void removeRigid(RigidJoint* r);

		void addMotor(MotorJoint* m);
		void removeMotor(MotorJoint* m);

		void addEdge(Edge* e);
		void removeEdge(Edge* e);

		void addAnchor(Anchor*);
		void removeAnchor(Anchor* a);

		void addPrimitive(Primitive*);
		void removePrimitive(Primitive* p);

		void destroyClumpGuts(Clump* c);
		void destroyClump(Clump* c);

		void destroyAssembly(Assembly* a);
		void destroyInfluence(Influence*);

		void addAssemblyEdges(Assembly*);

		void removeExternalEdge(Edge* e);
		void removeInternalEdge(Edge* e);
		void removeAssemblyEdges(Assembly* a);

		bool inClump(Primitive*);
		bool inBuffers(RigidJoint* r);

		bool removeFromBuffers(RigidJoint* r);
		void removeFromClump(Clump* c, RigidJoint* r);
		void removeSpanningTreeFast(Primitive* topPrim, RigidJoint* toParent);
		void removeFromClumpFast(Primitive* p, RigidJoint* toParent);
		void removeFromAssemblyFast(Primitive* p);

		void updateMotorJoint(MotorJoint* m);

		bool validateAll();
		bool validateEdge(Edge*);
		bool validateRigid(RigidJoint*);
		bool validateMotor(MotorJoint*);
		bool validateExternal(Edge*);
		bool validatePrimitive(Primitive*);
		bool validateAnchor(Anchor*);
		bool validateClump(Clump*);
		bool validateAssembly(Assembly*);
		bool validateInfluence(Influence*);

		bool upToDate();
	public:
		ClumpStage(IStage* upstream, World* world);
		virtual ~ClumpStage();
	public:
		virtual IStage::StageType getStageType()
		{
			return CLUMP_STAGE;
		}
		World* getWorld();
		virtual void onEdgeAdded(Edge* e);
		virtual void onEdgeRemoving(Edge* e);
		virtual int getMetric(MetricType metricType);
		void onPrimitiveAdded(Primitive* p);
		void onPrimitiveRemoving(Primitive* p);
		virtual void stepWorld(int worldStepId, int uiStepId, bool throttling);
		void stepUi(int uiStepId);
		void process();
		void onPrimitiveAddedAnchor(Primitive* p);
		void onPrimitiveRemovingAnchor(Primitive* p);
		void onPrimitiveCanSleepChanged(Primitive* p);
		void onPrimitiveCanCollideChanged(Primitive* p);
		void onMotorAngleChanged(MotorJoint* m);
  
	private:
		static PrimitiveSort getRigidPower(RigidJoint* r);
	public:
		static PrimitiveSort getMotorPower(const MotorJoint* m);
	private:
		static int numClumps(RigidJoint* r);
	};
}
