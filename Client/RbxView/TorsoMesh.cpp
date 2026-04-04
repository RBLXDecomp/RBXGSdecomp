#include "TorsoMesh.h"

class DeformFrontBack
{
	const float inset;
	const float shoulderInset;
public:
	DeformFrontBack(float inset, float shoulderInset)
		: inset(inset),
		  shoulderInset(shoulderInset)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class DeformSide
{
	const float inset;
	const float shoulderInset;
public:
	DeformSide(float inset, float shoulderInset)
		: inset(inset),
		  shoulderInset(shoulderInset)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class DeformTop
{
	const float inset;
	const float shoulderInset;
public:
	DeformTop(float inset, float shoulderInset)
		: inset(inset),
		  shoulderInset(shoulderInset)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

void TorsoBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_X>(DeformSide(bevel, shoulderInset), G3D::Vector2int16(1, 1), purpose);
}

void TorsoBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y>(DeformTop(bevel, shoulderInset), G3D::Vector2int16(1, 1), purpose);
}

void TorsoBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z>(DeformFrontBack(bevel, shoulderInset), G3D::Vector2int16(1, 1), purpose);
}

void TorsoBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_X_NEG>(DeformSide(bevel, shoulderInset), G3D::Vector2int16(1, 1), purpose);
}

void TorsoBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y_NEG>(*this, 1, 1, purpose);
}

void TorsoBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z_NEG>(DeformFrontBack(bevel, shoulderInset), G3D::Vector2int16(1, 1), purpose);
}

static const float bevel = 0.065f;

namespace RBX
{
	namespace View
	{
		TorsoMesh::TorsoMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes)
		{
			float shoulderInset = size.z * 0.3;

			levels.push_back(new Render::Mesh::Level(G3D::RenderDevice::QUADS));

			TorsoBuilder builder(levels.last(), size, surfaceTypes, 0.0f, shoulderInset);
			builder.build(LevelBuilder::Surface);
		}

		TorsoMesh::TorsoMesh(const G3D::Vector3& size, NormalId decalFace)
		{
			float shoulderInset = size.z * 0.3;

			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			TorsoBuilder builder(level, size, RenderSurfaceTypes(), bevel, shoulderInset);
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		TorsoMesh::TorsoMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile)
		{
			float shoulderInset = size.z * 0.3;

			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			TorsoBuilder builder(level, size, RenderSurfaceTypes(), bevel, shoulderInset);
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}
	}
}
