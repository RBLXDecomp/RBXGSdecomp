#pragma once
#include "RenderLib/Mesh.h"
#include "util/MeshId.h"

namespace RBX
{
	namespace View
	{
		class FileMesh : public Render::Mesh
		{
		public:
			FileMesh() {}
			bool loadFromMeshFile(const G3D::Vector3& scale, const MeshId meshFile);
		public:
			static G3D::ReferenceCountedPointer<Render::Mesh> create(const G3D::Vector3& scale, const MeshId meshFile);
		};
	}
}
