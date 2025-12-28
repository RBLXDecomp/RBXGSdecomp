#pragma once
#include "RenderLib/Chunk.h"
#include <G3D/Array.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/MeshAlg.h>
#include <G3D/ReferenceCount.h>
#include <G3D/Vector2.h>
#include <G3D/Vector3.h>
#include <GLG3D/RenderDevice.h>
#include <GLG3D/VAR.h>
#include <vector>

namespace RBX
{
	namespace Render
	{
		class Chunk;

		class Mesh : public G3D::ReferenceCountedObject
		{
		public:
			class Level : public G3D::ReferenceCountedObject
			{
			public:
				G3D::Array<unsigned> indexArray;
				G3D::RenderDevice::Primitive primitive;
			public:
				void appendVertex(const G3D::Vector3&, const G3D::Vector3&, const G3D::Vector2&);
				//Level(const Level&);
				Level();

				Level(G3D::RenderDevice::Primitive primitive)
					: primitive(primitive)
				{
				}

				virtual ~Level();
				//Level& operator=(const Level&);
			};

			class ShadowSurface
			{
			public:
				G3D::Array<G3D::MeshAlg::Edge> edgeArray;
				G3D::Array<G3D::Vector3> faceNormalArray;
				G3D::Array<G3D::MeshAlg::Face> faceArray;
				G3D::Array<G3D::Vector3> geometry;
				G3D::ReferenceCountedPointer<Level> dropShadowGeometry;
			public:
				//ShadowSurface(const ShadowSurface&);
				ShadowSurface();
				~ShadowSurface();
				//ShadowSurface& operator=(const ShadowSurface&);
			};

		protected:
			G3D::Array<G3D::ReferenceCountedPointer<Level>> levels;
			ShadowSurface shadowSurface;
		public:
			float debugBoundingRadius;
		private:
			static G3D::MeshAlg::Geometry visibleGeometry;
			static G3D::Array<G3D::Vector2> texCoordArray;
			static G3D::Array<G3D::Vector3> tangentArray;
			static G3D::Array<int> vertexRefCounts;
			static G3D::Array<int> freeList;
			static G3D::VAR vertexVAR;
			static G3D::VAR normalVAR;
			static G3D::VAR texCoordVAR;
			static G3D::VAR tangentVAR;
			static bool varDirty;
			static G3D::Vector3 findScale;
			static G3D::Array<int> findGrid[32][32][32];
		protected:
			int detailIndex(double) const;
			void computeShadowSurface(const G3D::ReferenceCountedPointer<Level>&);
		public:
			//Mesh(const Mesh&);
		protected:
			Mesh();
		public:
			virtual ~Mesh();
			G3D::ReferenceCountedPointer<Level> dropShadowMesh()
			{
				return shadowSurface.dropShadowGeometry;
			}
			const G3D::ReferenceCountedPointer<Level> detailLevel(float) const;
			void computeDirectionalShadowVolume(const G3D::CoordinateFrame& cframe, const G3D::Vector3& wsLightVector, G3D::Array<unsigned>& indexArray, G3D::Array<G3D::Vector3>& shadowVertex, bool generateLightCap) const;
			//Mesh& operator=(const Mesh&);

		private:
			static void makeVAR();
			static void computeAABounds(const std::vector<G3D::ReferenceCountedPointer<Chunk>>&, G3D::Vector3&, G3D::Vector3&);
		public:
			static void flushVAR();
		protected:
			static void initStatics();
		public:
			static unsigned allocVertex(unsigned i, unsigned count);
			static unsigned allocVertex(const G3D::Vector3& vertex, const G3D::Vector3& normal, const G3D::Vector2& tex, bool share);
			static G3D::ReferenceCountedPointer<Mesh> aggregate(const std::vector<G3D::ReferenceCountedPointer<Chunk>>&, G3D::CoordinateFrame&, float&);
		private:
			static G3D::Array<int>& getFindGridCell(const G3D::Vector3&);
		public:
			static unsigned findVertex(const G3D::Vector3&, const G3D::Vector3&, const G3D::Vector2&);
		private:
			static unsigned findVertex(const G3D::Array<int>&, const G3D::Vector3&, const G3D::Vector3&, const G3D::Vector2&);
			static void toFindGridCoords(const G3D::Vector3&, int&, int&, int&);
		public:
			static void freeVertex(unsigned index);
			static void setShadowVertex(const G3D::Vector3&);
			static void beginRender(G3D::RenderDevice* rd, bool usetexCoords, bool useTangent);
			static void sendGeometry(const Level* lvl, G3D::RenderDevice* rd);
			static void endRender(G3D::RenderDevice* rd);
		};
	}
}
