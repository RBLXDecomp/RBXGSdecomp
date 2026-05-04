#include "FileMesh.h"
#include <algorithm>
#include "util/ContentProvider.h"
#include <cstdio>

namespace RBX
{
	namespace View
	{
		//44.36% matching
		//a *start*
		bool FileMesh::loadFromMeshFile(const G3D::Vector3 &scale, const RBX::MeshId meshFile)
		{
			std::string fileName;
			int num_faces;
			if (!RBX::ContentProvider::singleton().requestContentFile(meshFile, fileName))
			{
				return false;
			}
			Render::Mesh::Level* level = new Render::Mesh::Level(G3D::RenderDevice::TRIANGLES);
			levels.append(level);

			FILE* fp = fopen(fileName.c_str(), "r");
			fscanf(fp, "version 1.00\n");
			fscanf(fp, "%d\n", num_faces);

			std::vector<unsigned int> verts;
			for (int i = 0; i < num_faces; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					float vtxX, vtxY, vtxZ, normX, normY, normZ, texU, texV, texW;
					fscanf(fp, "[%f,%f,%f][%f,%f,%f][%f,%f,%f]", &vtxX, &vtxY, &vtxZ, &normX, &normY, &normZ, &texU, &texV, &texW);
					G3D::Vector3 vert(vtxX * 0.5f, vtxY * 0.5f, vtxY * 0.5f);
					G3D::Vector2 tex(texU, 1.0f - texV);
					G3D::Vector3 norm(normX, normY, normZ);
					vert *= scale;
					unsigned int vtx1 = allocVertex(vert, norm, tex, true);
					verts.push_back(vtx1);
					unsigned int vtx2 = allocVertex(i, 1);
					verts.push_back(vtx2);
					unsigned int vtx3 = allocVertex(i, 1);
					verts.push_back(vtx3);
					unsigned int vtx4 = allocVertex(i, 1);
					verts.push_back(vtx4);
					level->indexArray.append(vtx1, vtx2, vtx3, vtx4);

					for (int k = 0; k < 3; k++)
					{
						freeVertex(verts[k]);
					}

					verts.clear();
				}
			}
			return true;
		}

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
