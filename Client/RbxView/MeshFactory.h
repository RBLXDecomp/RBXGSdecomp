#pragma once
#include "QuadVolume.h"
#include <G3D/Table.h>
#include <map>

namespace RBX
{
	namespace View
	{
		template<class Mesh, size_t numMeshes>
		class MeshFactory
		{
			class DecalKey
			{
			private:
				G3D::Vector3 size;
				NormalId face;
			public:
				DecalKey(NormalId, const G3D::Vector3&);
				bool operator<(const DecalKey&) const;
			};

			class TextureKey : protected DecalKey
			{
			private:
				G3D::Vector2 studsPerTile;
			public:
				TextureKey(NormalId, const G3D::Vector3&, const G3D::Vector2&);
				bool operator<(TextureKey&) const;
			};

			struct Variations
			{
				G3D::ReferenceCountedPointer<Render::Mesh> meshes[numMeshes];
			};

		private:
			static G3D::Table<RenderSurfaceTypes, G3D::Table<G3D::Vector3, Variations>> cache1;
			static std::map<DecalKey, G3D::ReferenceCountedPointer<Render::Mesh>> decalCache;
			static std::map<TextureKey, G3D::ReferenceCountedPointer<Render::Mesh>> textureCache;
		public:
			static Mesh* create(const G3D::Vector3&, RenderSurfaceTypes);
			static Mesh* createDecal(const G3D::Vector3&, NormalId);
			static Mesh* createTexture(const G3D::Vector3&, NormalId, const G3D::Vector2&);
		};
	}
}
