#include "FileMesh.h"
#include <algorithm>

namespace RBX
{
	namespace View
	{
		G3D::ReferenceCountedPointer<Render::Mesh> FileMesh::create(const G3D::Vector3& scale, const MeshId meshFile)
		{
			FileMesh* newMesh = new FileMesh;
			G3D::ReferenceCountedPointer<FileMesh> mesh = newMesh;

			if (!newMesh->loadFromMeshFile(scale, meshFile))
				return NULL;
			else
				return mesh;
		}
	}
}
