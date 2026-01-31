#pragma once
#include "QuadVolume.h"

namespace RBX
{
	namespace View
	{
		class BevelMesh : public Render::Mesh
		{
		public:

			class Builder : public LevelBuilder
			{
			protected:
				const float bevel;
			protected:
				Builder(G3D::ReferenceCountedPointer<Render::Mesh::Level>& level, G3D::Vector3 size, RenderSurfaceTypes surfaceTypes, float bevel)
					: LevelBuilder(level, size, surfaceTypes),
					  bevel(bevel)
				{
				}
			public:
				virtual void build(Purpose purpose);
				void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
			};
		};
	}
}
