#pragma once
#include "v8world/Geometry.h"
#include "v8kernel/Pair.h"
#include <G3D/Vector3int16.h>

namespace RBX
{
	class Block : public Geometry
	{
	private:
		const G3D::Vector3* vertices;
		float cornerRadius;
	private:
		static const int BLOCK_FACE_TO_VERTEX[6][4];
		static const int BLOCK_FACE_VERTEX_TO_EDGE[6][4];
	private:
		const G3D::Vector3* getCornerPoint(const G3D::Vector3int16& clip) const;
		const G3D::Vector3* getEdgePoint(const G3D::Vector3int16& clip, NormalId& normalID) const;
		const G3D::Vector3* getPlanePoint(const G3D::Vector3int16& clip, NormalId& normalID) const;
		G3D::Matrix3 getMomentSolid(float) const;
		G3D::Matrix3 getMomentHollow(float) const;
		virtual void onSetSize();
	public:
		Block(const Block& other);
		Block() : Geometry(), cornerRadius(0.0f), vertices(NULL) {}
		virtual ~Block() {}
		virtual bool hitTest(const G3D::Ray&, G3D::Vector3&, bool&);
		virtual GeometryType getGeometryType() const
		{
			return GEOMETRY_BLOCK;
		}
		virtual float getRadius() const;
		virtual G3D::Vector3 getCenterToCorner(const G3D::Matrix3& rotation) const;
		virtual G3D::Matrix3 getMoment(float mass) const;
		virtual float getGridVolume() const;
		void projectToFace(G3D::Vector3& ray, G3D::Vector3int16& clip, int& onBorder);
		GeoPairType getBallInsideInfo(const G3D::Vector3& ray, const G3D::Vector3*& offset, NormalId& normalID);
		GeoPairType getBallBlockInfo(int onBorder, const G3D::Vector3int16 clip, const G3D::Vector3*& offset, NormalId& normalID);
		const float* getVertices() const { return (float*)this->vertices; }
		const G3D::Vector3& getExtent() const;
		const G3D::Vector3* getFaceVertex(NormalId normId, int edge) const
		{
			return &this->vertices[Block::BLOCK_FACE_TO_VERTEX[normId][edge]];
		}
		int getClosestEdge(const G3D::Matrix3& rotation, NormalId normalID, G3D::Vector3& crossAxis);
		int faceVertexToEdge(NormalId normId, int edge)
		{
			return BLOCK_FACE_VERTEX_TO_EDGE[normId][edge];
		}
		int faceVertexToClockwiseEdge(NormalId, int);
		const G3D::Vector3* getEdgeVertex(int edgeId) const;
		NormalId getEdgeNormal(int edge)
		{
			int answer = edge / 4 + 3 * (edge % 2);
			if (edge > 12)
				answer = (answer + 3) % 6;
			return (NormalId)answer;
		}
		G3D::Vector2 getProjectedVertex(const G3D::Vector3& vertex, NormalId normalID);
		Block& operator=(const Block& other);

		// TODO: remove
		void matchDummy();
	};
}