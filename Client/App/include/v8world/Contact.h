#pragma once
#include <G3D/Vector3int16.h>
#include "v8kernel/Connector.h"
#include "v8world/Edge.h"
#include "v8world/Primitive.h"
#include "v8world/Ball.h"
#include "v8world/Block.h"

namespace RBX
{
	class Contact : public Edge
	{
	private:
		int lastContactStep;
		int steppingIndex;
		float jointK;
		float elasticJointK;
		float kFriction;
	private:
		static bool ignoreBool;
	protected:
		static int contactPairMatches;
		static int contactPairMisses;
  
	private:
		virtual void putInKernel(Kernel* _kernel);
		virtual void removeFromKernel();
		virtual Edge::EdgeType getEdgeType() const
		{
			return CONTACT;
		}
	protected:
		Body* getBody(int) const;
		ContactConnector* createConnector();
		void deleteConnector(ContactConnector*& c);
		void deleteConnectorInline(ContactConnector*& c); //for matching only
		virtual void deleteAllConnectors() = 0;
		virtual bool stepContact() = 0;
	public:
		Contact(Primitive* prim0, Primitive* prim1);
		virtual ~Contact() {}
	public:
		int& steppingIndexFunc()
		{
			return steppingIndex;
		}
		virtual bool computeIsColliding(float) = 0;
		bool computeIsAdjacent(float spaceAllowed);
		void onPrimitiveContactParametersChanged();
		bool step(int uiStepId);
  
	public:
		static bool isContact(Edge*);
	};

	class BallBallContact : public Contact
	{
	private:
		ContactConnector* ballBallConnector;

		Ball* ball(int i);
		virtual void deleteAllConnectors();
		virtual bool computeIsColliding(float overlapIgnored);
		virtual bool stepContact();
	public:
		BallBallContact(Primitive* p0, Primitive* p1);
		virtual ~BallBallContact();
	};

	class BallBlockContact : public Contact
	{
	private:
		ContactConnector* ballBlockConnector;

		Primitive* ballPrim();
		Primitive* blockPrim();
		Ball* ball();
		Block* block();
		virtual bool computeIsColliding(float overlapIgnored);
		bool computeIsColliding(int& onBoarder, G3D::Vector3int16& clip, G3D::Vector3& projectionInBlock, float overlapIgnored);
		virtual void deleteAllConnectors();
		virtual bool stepContact();
	public:
		BallBlockContact(Primitive* p0, Primitive* p1);
		virtual ~BallBlockContact();
	};


	class BlockBlockContact : public Contact
	{
	private:
		std::vector<ContactConnector *,std::allocator<ContactConnector *> > connectors;
		std::vector<bool,std::allocator<bool> > matched;
		int separatingAxisId;
		int separatingBodyId;
		int feature[2];
		int bPlane;
		int bOther;
		NormalId planeID;
		NormalId otherPlaneID;

		ContactConnector* matchContactConnector(Body* b0, Body* b1, GeoPairType _pairType, int param0, int param1);
		void deleteUnmatchedConnectors();
		void loadGeoPairEdgeEdge(int b0, int b1, int edge0, int edge1);
		Block* block(int i);
		bool getBestPlaneEdge(bool& planeContact, float overlapIgnored);
		int computePlaneContact();
		int intersectRectQuad(G3D::Vector2& planeRect, G3D::Vector2 (&otherQuad)[4]); //might not be correct?
		void loadGeoPairPointPlane(int pointBody, int planeBody, int pointID, NormalId pointFaceID, NormalId planeFaceID);
		void loadGeoPairEdgeEdgePlane(int edgeBody, int planeBody, int edge0, int edge1);
		virtual bool computeIsColliding(float overlapIgnored);
		bool computeIsColliding(bool& planeContact, float overlapIgnored);
		virtual void deleteAllConnectors();
		virtual bool stepContact();
	public:
		BlockBlockContact(Primitive* p0, Primitive* p1);
		virtual ~BlockBlockContact();
		static float contactPairHitRatio();
	};
}
