#include "HeadMesh.h"

class CylinderTransform
{
	const float bevel;
	const int halfVertexCount;
public:
	CylinderTransform(float bevel, int halfVertexCount)
		: bevel(bevel),
		halfVertexCount(halfVertexCount)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class EndcapTransform
{
	const float bevel;
public:
	EndcapTransform(float bevel)
		: bevel(bevel)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

//72.86% matching
//Weird stuff with the size setting
inline G3D::Vector3 getSizeMax(const G3D::Vector3& v) {
	float maxSizeX = G3D::min(v.x, v.z);
	float maxSizeZ = G3D::min(v.x, v.z);

	G3D::Vector3 newVec;
	newVec.x = maxSizeX;
	newVec.y = v.y;
	newVec.z = maxSizeZ;

	return newVec;
}

HeadBuilder::HeadBuilder(G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level> level, G3D::Vector3 size, RBX::View::RenderSurfaceTypes surfaceTypes, size_t elements, float bevel)
: LevelBuilder(level, getSizeMax(size), surfaceTypes),
	elements(elements),
	bevel(bevel)
{
}

void HeadBuilder::buildTop(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_Y>(EndcapTransform(bevel), G3D::Vector2int16(e, e), purpose);
}

void HeadBuilder::buildBottom(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_Y_NEG>(EndcapTransform(bevel), G3D::Vector2int16(e, e), purpose);
}

//77.81% matching
void HeadBuilder::buildLeft(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_X_NEG>(CylinderTransform(bevel, e / 2), G3D::Vector2int16(e, e), purpose);
}

//77.81% matching
void HeadBuilder::buildRight(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_X>(CylinderTransform(bevel, e / 2), G3D::Vector2int16(e, e), purpose);
}

//77.81% matching
void HeadBuilder::buildFront(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_Z_NEG>(CylinderTransform(bevel, e / 2), G3D::Vector2int16(e, e), purpose);
};

//77.81% matching
void HeadBuilder::buildBack(RBX::View::LevelBuilder::Purpose purpose) {
	int e = elements;
	buildFace<RBX::NORM_Z>(CylinderTransform(bevel, e / 2), G3D::Vector2int16(e, e), purpose);
}

namespace RBX {
	namespace View {

		HeadMesh::HeadMesh(const G3D::Vector3& size, NormalId decalFace) 
		{
			float bevel = G3D::min(size.x, size.z);
			bevel *= 0.25f;
			Render::Mesh::Level* level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			HeadBuilder builder(level, size, RenderSurfaceTypes(), 9, bevel);
			builder.buildFace(decalFace, LevelBuilder::Decal);
		}

		HeadMesh::HeadMesh(const G3D::Vector3& size, NormalId textureFace, const G3D::Vector2& studsPerTile) 
		{
			float bevel = G3D::min(size.x, size.z);
			bevel *= 0.25f;
			Render::Mesh::Level* level = new Render::Mesh::Level(G3D::RenderDevice::QUADS);
			levels.push_back(level);

			HeadBuilder builder(level, size, RenderSurfaceTypes(), 9, bevel);
			builder.textureScale = mapToUvw_Legacy(size, textureFace).xy() * 2.0f / studsPerTile;

			builder.buildFace(textureFace, LevelBuilder::Decal);
		}

		HeadMesh::HeadMesh(const G3D::Vector3& partSize, RenderSurfaceTypes surfaceTypes) 
		{
			float bevel = G3D::min(partSize.x, partSize.z);
			bevel *= 0.25f;
			levels.push_back(new Render::Mesh::Level(G3D::RenderDevice::QUADS));

			G3D::ReferenceCountedPointer<Render::Mesh::Level> level = levels.last();
			HeadBuilder builder(level, partSize, surfaceTypes, 9, bevel);
			builder.build(LevelBuilder::Surface);
		}
	};
};