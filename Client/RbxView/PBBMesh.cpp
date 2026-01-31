#include "PBBMesh.h"
#include "BrickMesh.h"
#include "ViewBase.h"
#include <cstdlib>

class Face
{
	G3D::Vector2 perturbation;
	bool bulge[2];
public:
	Face()
	{
		perturbation.x = G3D::uniformRandom(-RBX::View::BrickMesh::normalPerturbation, RBX::View::BrickMesh::normalPerturbation);
		perturbation.y = G3D::uniformRandom(-RBX::View::BrickMesh::normalPerturbation, RBX::View::BrickMesh::normalPerturbation);
		bulge[0] = G3D::uniformRandom() > 0.5f;
		bulge[1] = G3D::uniformRandom() > 0.5f;
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class Side : public Face
{
public:
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

PBBBuilder::PBBBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes)
	: LevelBuilder(level, size, surfaceTypes)
{
	strips.x = G3D::max<int>(1, size.x / 6.0f);
	strips.y = G3D::max<int>(1, size.y / 6.0f);
	strips.z = G3D::max<int>(1, size.z / 6.0f);

	strips.x = G3D::min<int>(4, strips.x);
	strips.y = G3D::min<int>(4, strips.y);
	strips.z = G3D::min<int>(4, strips.z);
}

void PBBBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose)
{
	Side deform;
	buildFace<RBX::NORM_X>(deform, G3D::Vector2int16(1, 1), purpose);
}

void PBBBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose)
{
	Face deform;
	buildFace<RBX::NORM_Y>(deform, G3D::Vector2int16(1, 1), purpose);
}

void PBBBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose)
{
	Side deform;
	buildFace<RBX::NORM_Z>(deform, G3D::Vector2int16(1, 1), purpose);
}

void PBBBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose)
{
	Side deform;
	buildFace<RBX::NORM_X_NEG>(deform, G3D::Vector2int16(1, 1), purpose);
}

void PBBBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose)
{
	Face deform;
	buildFace<RBX::NORM_Y_NEG>(deform, G3D::Vector2int16(1, 1), purpose);
}

void PBBBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose)
{
	Side deform;
	buildFace<RBX::NORM_Z_NEG>(deform, G3D::Vector2int16(1, 1), purpose);
}

namespace RBX
{
	namespace View
	{
		PBBMesh::PBBMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes)
		{
			{
				G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
				levels.push_back(level);

				PBBBuilder builder(level, size, surfaceTypes);
				builder.build(LevelBuilder::Surface);
			}

			if (ViewBase::getShadowsEnabled())
			{
				G3D::ReferenceCountedPointer<Render::Mesh::Level> tempShadowLevel = new Render::Mesh::Level(G3D::RenderDevice::TRIANGLES);

				PBBBuilder builder(tempShadowLevel, size, surfaceTypes);
				builder.build(LevelBuilder::Shadow);

				computeShadowSurface(tempShadowLevel);
			}
		}

		PBBMesh::PBBMesh(const G3D::Vector3& size, NormalId decalFace)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			PBBBuilder builder(level, size, RenderSurfaceTypes());
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		PBBMesh::PBBMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			PBBBuilder builder(level, size, RenderSurfaceTypes());
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}
	}
}
