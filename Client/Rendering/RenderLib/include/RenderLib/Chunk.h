#pragma once
#include "RenderLib/Material.h"
#include "RenderLib/Mesh.h"
#include "RenderLib/RenderStats.h"
#include <G3D/GLight.h>

namespace RBX
{
	namespace Render
	{
		class AggregateChunk;
		class Mesh;

		class __declspec(novtable) Chunk : public G3D::ReferenceCountedObject
		{
		public:
			const float polygonOffset;
			float radius;
			G3D::WeakReferenceCountedPointer<AggregateChunk> aggregate;
		public:
			static const float DECAL_OFFSET;
			static const float TEXTURE_OFFSET;
		public:
			virtual bool cachesShadows() const
			{
				return false;
			}
			virtual void renderShadows(G3D::RenderDevice* rd, const G3D::GLight& light, bool caps, float shadowVertexDistance)
			{
				return;
			}
			virtual bool castsShadows() const;
			virtual bool cullable() const;
			virtual G3D::ReferenceCountedPointer<Material> getMaterial();
			virtual G3D::ReferenceCountedPointer<Mesh> getMesh();
			virtual const G3D::CoordinateFrame& cframe();
			//Chunk(const Chunk&);

			Chunk(float polygonOffset)
				: polygonOffset(polygonOffset),
				radius(G3D::inf())
			{
				RenderStats::chunkCount++;
			}

			virtual ~Chunk()
			{
				RenderStats::chunkCount--;
			}
		};

		class AggregateChunk : public Chunk
		{
		private:
			G3D::ReferenceCountedPointer<Material> material;
			G3D::ReferenceCountedPointer<Mesh> mesh;
			G3D::CoordinateFrame _cframe;
			bool _castsShadows;
			bool _cullable;
			G3D::GLight shadowSource;
			G3D::VAR shadowVAR;
			G3D::Array<unsigned int> shadowIndexArray;
			G3D::Array<unsigned short> shadowIndexArray16;
		public:
			std::vector<G3D::ReferenceCountedPointer<Chunk>> components;
		public:
			static bool randomColors;
			static const G3D::CoordinateFrame identity;
		public:
			virtual bool castsShadows() const
			{
				return _castsShadows;
			}
			virtual bool cullable() const
			{
				return _cullable;
			}
			virtual void renderShadows(G3D::RenderDevice* rd, const G3D::GLight& light, bool caps, float shadowVertexDistance);
			//AggregateChunk(const AggregateChunk&);
			AggregateChunk(const G3D::ReferenceCountedPointer<Chunk>& firstChunk);
			virtual ~AggregateChunk();
			virtual bool cachesShadows() const;
			void makeMesh();
			virtual G3D::ReferenceCountedPointer<Material> getMaterial()
			{
				return material;
			}
			virtual G3D::ReferenceCountedPointer<Mesh> getMesh()
			{
				return mesh;
			}
			virtual const G3D::CoordinateFrame& cframe()
			{
				return _cframe;
			}
		};
	}
}
