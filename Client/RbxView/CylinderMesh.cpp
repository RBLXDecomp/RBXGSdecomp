#include "CylinderMesh.h"

// TODO: these functions are static. make them static once they're defined
void CylinderTransform(G3D::Vector3& vertex, G3D::Vector3& normal, G3D::Vector2& tex, const G3D::Vector3& radius, const G3D::Vector2int16& ivertex);

void EndcapTransform(G3D::Vector3& vertex, G3D::Vector3& normal, G3D::Vector2& tex, const G3D::Vector3& radius, const G3D::Vector2int16& ivertex);

CylinderBuilder::CylinderBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>& level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, size_t slices)
	: LevelBuilder(level, size, surfaceTypes),
	  elements(G3D::max<size_t>(slices / 4, 1))
{
}

void CylinderBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_X>(&EndcapTransform, G3D::Vector2int16(e, e), purpose);
}

void CylinderBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y>(&CylinderTransform, G3D::Vector2int16(elements, 1), purpose);
}

void CylinderBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z>(&CylinderTransform, G3D::Vector2int16(elements, 1), purpose);
}

void CylinderBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_X_NEG>(&EndcapTransform, G3D::Vector2int16(e, e), purpose);
}

void CylinderBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Y_NEG>(&CylinderTransform, G3D::Vector2int16(elements, 1), purpose);
}

void CylinderBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose)
{
	buildFace<RBX::NORM_Z_NEG>(&CylinderTransform, G3D::Vector2int16(elements, 1), purpose);
}

namespace RBX
{
	namespace View
	{
		CylinderAlongXMesh::CylinderAlongXMesh(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes)
		{
			levels.push_back(new Render::Mesh::Level(G3D::RenderDevice::QUADS));

			G3D::ReferenceCountedPointer<Render::Mesh::Level>& lastLevel = levels.last();

			CylinderBuilder builder(lastLevel, size, surfaceTypes, 12);
			builder.build(LevelBuilder::Surface);
		}

		CylinderAlongXMesh::CylinderAlongXMesh(const G3D::Vector3& size, NormalId decalFace)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			CylinderBuilder builder(level, size, RenderSurfaceTypes(), 12);
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		CylinderAlongXMesh::CylinderAlongXMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			CylinderBuilder builder(level, size, RenderSurfaceTypes(), 12);
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}
	}
}
