#pragma once
#include "QuadVolume.h"
#include "BevelMesh.h"
#include "MeshFactory.h"

class TorsoBuilder : public RBX::View::BevelMesh::Builder
{
public:
	const float shoulderInset;
public:
	TorsoBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, float bevel, float shoulderInset)
		: Builder(level, size, surfaceTypes, bevel),
		  shoulderInset(shoulderInset)
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
		class TorsoMesh : public BevelMesh, public MeshFactory<TorsoMesh, 1>
		{
		private:
			Render::Mesh::Level& addTorso(float, float);

			TorsoMesh(const G3D::Vector3&, NormalId, const G3D::Vector2&);
			TorsoMesh(const G3D::Vector3&, NormalId);
			TorsoMesh(const G3D::Vector3&, RenderSurfaceTypes);
		};
	}
}
