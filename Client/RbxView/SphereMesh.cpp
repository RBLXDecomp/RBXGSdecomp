#include "SphereMesh.h"
#include "ViewBase.h"
#include "util/Math.h"

static void SphereTransform(G3D::Vector3& vertex, G3D::Vector3& normal, G3D::Vector2& tex, const G3D::Vector3& radius, const G3D::Vector2int16& ivertex)
{
	G3D::Vector3 newNormal = vertex.direction();
	normal = newNormal;
	vertex = newNormal * radius;
}

void SphereBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_X>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

void SphereBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_Y>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

void SphereBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_Z>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

void SphereBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_X_NEG>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

void SphereBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_Y_NEG>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

void SphereBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose)
{
	int e = elements;
	buildFace<RBX::NORM_Z_NEG>(&SphereTransform, G3D::Vector2int16(e, e), purpose);
}

namespace RBX
{
	namespace View
	{
		// TODO: this seems to be required, otherwise putting this in the SphereMesh ctor overload right below only results in a 95% match.
		// is there a better way of doing this?
		static __forceinline void doBuild(G3D::ReferenceCountedPointer<Render::Mesh::Level>& level, const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes, int elements)
		{
			SphereBuilder builder(level, size, surfaceTypes, elements);
			builder.build(LevelBuilder::Surface);
		}

		SphereMesh::SphereMesh(const G3D::Vector3& diameter, RenderSurfaceTypes surfaceTypes)
		{
			levels.push_back(new Render::Mesh::Level(G3D::RenderDevice::QUADS));

			doBuild(levels.last(), diameter, surfaceTypes, 6);

			if (ViewBase::getShadowsEnabled())
			{
				int elements = (diameter.x < 40.0f && diameter.y < 40.0f && diameter.z < 40.0f) ? 3 : 4;
				G3D::ReferenceCountedPointer<Render::Mesh::Level> tempShadowLevel = new Render::Mesh::Level(G3D::RenderDevice::TRIANGLES);

				SphereBuilder builder(tempShadowLevel, diameter, surfaceTypes, elements);
				builder.build(LevelBuilder::Shadow);

				computeShadowSurface(tempShadowLevel);
			}
		}

		//99.44% match
		SphereMesh::SphereMesh(const G3D::Vector3& size, NormalId decalFace)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			SphereBuilder builder(level, size, RenderSurfaceTypes(), 6);
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		SphereMesh::SphereMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile)
		{
			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			SphereBuilder builder(level, size, RenderSurfaceTypes(), 6);
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}
	}
}
