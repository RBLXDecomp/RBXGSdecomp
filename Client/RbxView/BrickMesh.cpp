#include "BrickMesh.h"
#include "ViewBase.h"

static std::map<Lookup, Variations> cache;

static int orderedCompare(const G3D::Vector3& a, const G3D::Vector3& b)
{
	if (a.x < b.x)
		return -1;
	if (a.x > b.x)
		return 1;
	
	if (a.y < b.y)
		return -1;
	if (a.y > b.y)
		return 1;

	if (a.z < b.z)
		return -1;
	if (a.z > b.z)
		return 1;

	return 0;
}

static void appendTriangles(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, int a, int b, int c, int d);

namespace RBX
{
	namespace View
	{
		const float BrickMesh::widthInset = 0.0f;
		const float BrickMesh::normalPerturbation = 0.05f;
		const float BrickMesh::bevel = 0.0f;

		void BrickMesh::flushCache()
		{
			cache.clear();
		}

		G3D::ReferenceCountedPointer<Render::Mesh> BrickMesh::create(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes, BrickColor color)
		{
			Lookup lookup = {size, surfaceTypes, color};

			Variations& variation = cache[lookup];
			
			size_t mesh = variation.count == 4 ? G3D::iRandom(0, 3) : G3D::min(3, G3D::iRandom(0, 8));

			G3D::ReferenceCountedPointer<Render::Mesh> thisMesh = variation.meshes[mesh];

			if (thisMesh.isNull())
			{
				thisMesh = new BrickMesh(size, surfaceTypes, color);
				variation.meshes[mesh] = thisMesh;
				variation.count++;
			}

			return thisMesh;
		}
	}
}
