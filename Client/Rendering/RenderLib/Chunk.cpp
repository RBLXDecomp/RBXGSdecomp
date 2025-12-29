#include "RenderLib/Chunk.h"

namespace RBX
{
	namespace Render
	{
		const float Chunk::DECAL_OFFSET = -0.2f;
		const float Chunk::TEXTURE_OFFSET = -0.1f;

		const G3D::CoordinateFrame AggregateChunk::identity;

		AggregateChunk::AggregateChunk(const G3D::ReferenceCountedPointer<Chunk>& firstChunk)
			: Chunk(firstChunk->polygonOffset),
			  _castsShadows(firstChunk->castsShadows()),
			  _cullable(firstChunk->cullable())
		{
			if (!randomColors)
				material = firstChunk->getMaterial();

			RenderStats::aggregateChunkCount++;
			RenderStats::chunkCount--;

			if (randomColors)
			{
				material = new Material;
				material->appendLevel(NULL, (G3D::Color3::wheelRandom() + G3D::Color3::white()) / 2.0f, 0.0f, 50.0f, 0.0f, 0.0f);
			}
		}

		AggregateChunk::~AggregateChunk()
		{
			RenderStats::chunkCount++;
			RenderStats::aggregateChunkCount--;
			RenderStats::aggregatedChunkCount -= components.size();
		}

		void AggregateChunk::makeMesh()
		{
			mesh = Mesh::aggregate(components, _cframe, radius);
		}

		void AggregateChunk::renderShadows(G3D::RenderDevice* rd, const G3D::GLight& light, bool caps, float shadowVertexDistance)
		{
			if (light != shadowSource)
			{
				shadowSource = light;
				shadowIndexArray.clear();
				shadowIndexArray16.clear();

				if (material->veryTransparent())
					return;

				static G3D::Array<G3D::Vector3> shadowVertex;

				shadowVertex.clear();
				shadowVertex.push_back(-light.position.xyz().direction() * shadowVertexDistance);

				G3D::Vector3 worldLight = light.position.xyz().direction();
				mesh->computeDirectionalShadowVolume(cframe(), worldLight, shadowIndexArray, shadowVertex, caps);

				if (shadowVertex.size() < 65536)
				{
					int size = shadowIndexArray.size();

					shadowIndexArray16.resize(size);
					for (int i = 0; i < size; i++)
						shadowIndexArray16[i] = shadowIndexArray[i];

					shadowIndexArray.clear();
				}

				G3D::ReferenceCountedPointer<G3D::VARArea> area = G3D::VARArea::create(shadowVertex.size() * 12, G3D::VARArea::WRITE_EVERY_FEW_FRAMES);
				shadowVAR = G3D::VAR(shadowVertex, area);
			}

			if (shadowIndexArray.size() > 0)
			{
				rd->beginIndexedPrimitives();
				rd->setVertexArray(shadowVAR);
				rd->sendIndices(G3D::RenderDevice::TRIANGLES, shadowIndexArray.size(), shadowIndexArray.begin());
				rd->endIndexedPrimitives();
			}
			else if (shadowIndexArray16.size() > 0)
			{
				rd->beginIndexedPrimitives();
				rd->setVertexArray(shadowVAR);
				rd->sendIndices(G3D::RenderDevice::TRIANGLES, shadowIndexArray16.size(), shadowIndexArray16.begin());
				rd->endIndexedPrimitives();
			}
		}
	}
}
