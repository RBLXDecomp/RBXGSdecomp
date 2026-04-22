#pragma once
#include "v8world/IPipelined.h"

namespace RBX
{
	class Primitive;

	namespace Sim
	{
		enum EdgeState
		{
			UNDEFINED,
			STEPPING,
			SLEEPING,
			TOUCHING,
			TOUCHING_SLEEPING
		};
	}

	class Edge : public IPipelined
	{
	public:
		enum EdgeType
		{
			JOINT,
			CONTACT
		};

	private:
		Sim::EdgeState edgeState;
		Primitive* prim0;
		Primitive* prim1;
		Edge* next0;
		Edge* next1;
		bool inEdgeList;
  
	public:
		//Edge(const Edge&);
		Edge(Primitive* prim0, Primitive* prim1);
		virtual ~Edge() {}

		bool getInEdgeList() const;
		void setInEdgeList(bool);
		virtual EdgeType getEdgeType() const = 0;
		Sim::EdgeState getEdgeState() const;
		void setEdgeState(Sim::EdgeState);
		Primitive* getPrimitive(int i) const
		{
			return *(&this->prim0 + i);
		}
		Primitive* otherPrimitive(int) const;
		Primitive* otherPrimitive(const Primitive* p) const
		{
			return p != prim0 ? prim0 : prim1;
		}
		int getPrimitiveId(const Primitive*) const;
		Edge* getNext(const Primitive* p) const
		{
			return p == prim0 ? next0 : next1;
		}
		void setNext(Primitive* p, Edge* e)
		{
			if (p == prim0)
				next0 = e;
			else
				next1 = e;
		}
		bool links(Primitive*, Primitive*) const;
		bool links(const Primitive* p) const
		{
			return p == prim0 || p == prim1;
		}
		virtual void setPrimitive(int i, Primitive* p);
		//Edge& operator=(const Edge&);
	};
}
