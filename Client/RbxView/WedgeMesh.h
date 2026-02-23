#pragma once
#include "QuadVolume.h"
#include "BevelMesh.h"
#include "MeshFactory.h"

class WedgeBuilder : public RBX::View::BevelMesh::Builder
{
public:
	WedgeBuilder::WedgeBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, float bevel)
		: Builder(level, size, surfaceTypes, bevel)
	{
	}
protected:
	virtual void buildRight(RBX::View::LevelBuilder::Purpose purpose);
	virtual void buildTop(RBX::View::LevelBuilder::Purpose purpose);
	virtual void buildBack(RBX::View::LevelBuilder::Purpose purpose);
	virtual void buildLeft(RBX::View::LevelBuilder::Purpose purpose);
	virtual void buildBottom(RBX::View::LevelBuilder::Purpose purpose);
	virtual void buildFront(RBX::View::LevelBuilder::Purpose purpose);
};

namespace RBX
{
	namespace View
	{
		class WedgeMesh : public BevelMesh, public MeshFactory<WedgeMesh, 1>
		{
		private:
			WedgeMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile);
			WedgeMesh(const G3D::Vector3& size, NormalId decalFace);
			WedgeMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes);
		};
	}
}
