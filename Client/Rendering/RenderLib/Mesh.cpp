#include "RenderLib/Mesh.h"
#include <GLG3D/VARArea.h>

namespace RBX
{
	namespace Render
	{
		static int vertexRefCount = 0;
		static bool init;

		Mesh::Level::~Level()
		{
			for (int i = 0; i < indexArray.size(); i++)
				freeVertex(indexArray[i]);
		}

		Mesh::Mesh()
		{
			if (!init)
			{
				initStatics();
				init = true;
			}

			debugBoundingRadius = 0.0f;
		}

		Mesh::~Mesh() {}

		void Mesh::endRender(G3D::RenderDevice* rd)
		{
			rd->endIndexedPrimitives();
		}

		unsigned Mesh::allocVertex(unsigned i, unsigned count)
		{
			vertexRefCounts[i] += count;
			vertexRefCount += count;
			return i;
		}

		void Mesh::sendGeometry(const Level* lvl, G3D::RenderDevice* rd)
		{
			if (lvl && lvl->indexArray.size() > 0)
			{
				G3D::RenderDevice::Primitive primitive = lvl->primitive;
				rd->sendIndices(primitive, lvl->indexArray.size(), lvl->indexArray.begin());
			}
		}

		void Mesh::initStatics()
		{
			visibleGeometry.vertexArray.resize(1);
			visibleGeometry.normalArray.resize(1);
			texCoordArray.resize(1);
			tangentArray.resize(1);
			vertexRefCounts.resize(1);
			vertexRefCounts.front() = 1;
			freeList.clear();
		}

		void Mesh::beginRender(G3D::RenderDevice* rd, bool usetexCoords, bool useTangent)
		{
			makeVAR();
			rd->beginIndexedPrimitives();

			if (vertexVAR.valid())
				rd->setVertexArray(vertexVAR);

			if (normalVAR.valid())
				rd->setVertexArray(normalVAR);

			if (usetexCoords && texCoordVAR.valid())
				rd->setTexCoordArray(0, texCoordVAR);

			if (useTangent && tangentVAR.valid())
				rd->setTexCoordArray(2, tangentVAR);
		}

		void Mesh::freeVertex(unsigned index)
		{
			if (vertexRefCounts.begin())
			{
				vertexRefCount--;
				int& refIndex = vertexRefCounts[index];

				if (refIndex == 1)
				{
					refIndex = 0;
					freeList.push_back(index);
				}
				else
				{
					refIndex--;
				}
			}
		}

		void Mesh::makeVAR()
		{
			if (varDirty)
			{
				G3D::ReferenceCountedPointer<G3D::VARArea> area = G3D::VARArea::create(visibleGeometry.vertexArray.size() * 44 + 1024, G3D::VARArea::WRITE_ONCE);

				vertexVAR = G3D::VAR(visibleGeometry.vertexArray, area);
				normalVAR = G3D::VAR(visibleGeometry.normalArray, area);
				texCoordVAR = G3D::VAR(texCoordArray, area);
				tangentVAR = G3D::VAR(tangentArray, area);

				varDirty = false;
			}
		}

		unsigned Mesh::allocVertex(const G3D::Vector3& vertex, const G3D::Vector3& normal, const G3D::Vector2& tex, bool share)
		{
			if (!varDirty)
				varDirty = true;

			if (freeList.size() == 0)
			{
				int index = visibleGeometry.vertexArray.size();

				visibleGeometry.vertexArray.push_back(vertex);
				visibleGeometry.normalArray.push_back(normal);
				texCoordArray.push_back(tex);
				tangentArray.push_back(G3D::Vector3::zero());

				vertexRefCounts.push_back(1);
				vertexRefCount++;

				return index;
			}
			else
			{
				int index = freeList.pop();

				visibleGeometry.vertexArray[index] = vertex;
				visibleGeometry.normalArray[index] = normal;
				texCoordArray[index] = tex;
				tangentArray[index] = G3D::Vector3::zero();

				vertexRefCount++;
				vertexRefCounts[index] = 1;

				return index;
			}
		}
	}
}
