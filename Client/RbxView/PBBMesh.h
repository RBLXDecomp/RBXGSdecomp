#pragma once
#include "QuadVolume.h"
#include "MeshFactory.h"

class PBBBuilder : public RBX::View::LevelBuilder
{
public:
	G3D::Vector3int16 strips;
public:
	PBBBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes);
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
		class PBBMesh : public Render::Mesh, public MeshFactory<PBBMesh, 4>
		{
		private:
			PBBMesh(const G3D::Vector3&, NormalId, const G3D::Vector2&);
			PBBMesh(const G3D::Vector3&, NormalId);
			PBBMesh(const G3D::Vector3&, RenderSurfaceTypes);
		};
	}
}
