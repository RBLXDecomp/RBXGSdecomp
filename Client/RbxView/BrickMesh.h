#include "QuadVolume.h"
#include "v8datamodel/BrickColor.h"

struct Lookup
{
	G3D::Vector3 size;
	RBX::View::RenderSurfaceTypes surfaceTypes;
	RBX::BrickColor color;

	bool operator==(const Lookup&) const;
	bool operator<(const Lookup&) const;
};

struct Variations
{
	int count;
	G3D::ReferenceCountedPointer<RBX::Render::Mesh> meshes[4];

	Variations();
};

template<RBX::NormalId id>
void meshFace(const G3D::Vector3&, float, RBX::View::RenderSurfaceTypes, float, G3D::ReferenceCountedPointer<RBX::Render::Mesh::Level>&);

namespace RBX
{
	namespace View
	{
		class BrickMesh : public Render::Mesh
		{
		public:
			static const float widthInset;
			static const float normalPerturbation;
			static const float bevel;
		private:
			BrickMesh(const G3D::Vector3&, RenderSurfaceTypes, BrickColor);
		public:
			static G3D::ReferenceCountedPointer<Render::Mesh> create(const G3D::Vector3& size, RenderSurfaceTypes surfaceTypes, BrickColor color);
			static void flushCache();
		};
	}
}
