#pragma once
#include "QuadVolume.h"
#include "MeshFactory.h"

class SphereBuilder : public RBX::View::LevelBuilder
{
public:
	const int elements;
public:
	SphereBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, int elements)
		: LevelBuilder(level, size, surfaceTypes),
		  elements(elements)
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
		class SphereMesh : public Render::Mesh, public MeshFactory<SphereMesh, 1>
		{
		private:
			SphereMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile);
			SphereMesh(const G3D::Vector3& size, NormalId decalFace);
			SphereMesh(const G3D::Vector3& diameter, RenderSurfaceTypes surfaceTypes);
		};
	}
}
