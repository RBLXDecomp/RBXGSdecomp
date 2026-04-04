#include "WedgeMesh.h"

class DeformFront
{
	const float inset;
public:
	DeformFront(float inset)
		: inset(inset)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class DeformSide3
{
	const float inset;
	const float flip;
public:
	DeformSide3(float inset, bool flip)
		: inset(inset),
		  flip(flip ? -1.0f : 1.0f)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class DeformTop3
{
	const float inset;
public:
	DeformTop3(float inset)
		: inset(inset)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

void WedgeBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_X>(DeformSide3(bevel, true), G3D::Vector2int16(1, 1), purpose);
}

void WedgeBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y>(DeformTop3(bevel), G3D::Vector2int16(1, 1), purpose);
}

void WedgeBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z>(*this, 1, 1, purpose);
}

void WedgeBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_X_NEG>(DeformSide3(bevel, false), G3D::Vector2int16(1, 1), purpose);
}

void WedgeBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y_NEG>(*this, 1, 1, purpose);
}

void WedgeBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z_NEG>(DeformFront(bevel), G3D::Vector2int16(1, 1), purpose);
}

static const float bevel = 0.065f;

namespace RBX
{
	namespace View
	{
		WedgeMesh::WedgeMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes)
		{
			levels.push_back(new Render::Mesh::Level(G3D::RenderDevice::QUADS));

			WedgeBuilder builder(levels.last(), size, surfaceTypes, 0.0f);
			builder.build(LevelBuilder::Surface);
		}

		WedgeMesh::WedgeMesh(const G3D::Vector3& size, NormalId decalFace)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			WedgeBuilder builder(level, size, RenderSurfaceTypes(), bevel);
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		WedgeMesh::WedgeMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			WedgeBuilder builder(level, size, RenderSurfaceTypes(), bevel);
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}
	}
}
