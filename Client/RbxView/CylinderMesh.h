#pragma once
#include "QuadVolume.h"
#include "MeshFactory.h"

class CylinderBuilder : public RBX::View::LevelBuilder
{
public:
	const size_t elements;
public:
	CylinderBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, size_t slices);
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
		class CylinderAlongXMesh : public Render::Mesh, public MeshFactory<CylinderAlongXMesh, 1>
		{
		private:
			CylinderAlongXMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile);
			CylinderAlongXMesh(const G3D::Vector3& size, NormalId decalFace);
			CylinderAlongXMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes);
		};
	}
}
