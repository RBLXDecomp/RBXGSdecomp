#pragma once
#include "QuadVolume.h"
#include "MeshFactory.h"

class HeadBuilder : public RBX::View::LevelBuilder
{
public:
	const float bevel;
	const size_t elements;
public:
	HeadBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level> level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, size_t elements, float bevel);
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
		class HeadMesh : public Render::Mesh, public MeshFactory<HeadMesh, 1>
		{
		private:
			HeadMesh(const G3D::Vector3&, NormalId, const G3D::Vector2&);
			HeadMesh(const G3D::Vector3&, NormalId);
			HeadMesh(const G3D::Vector3&, RenderSurfaceTypes);
		};
	}
}
