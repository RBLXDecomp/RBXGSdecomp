#pragma once
#include "RenderLib/Mesh.h"
#include "RenderLib/Material.h"

namespace RBX
{
	namespace Render
	{
		class RenderSurface
		{
		public:
			G3D::CoordinateFrame cframe;
			float z;
			const Material::Level* material;
			float polygonOffset;
			const Mesh::Level* mesh;
			G3D::ReferenceCountedPointer<Mesh> fullMesh;
		public:
			//RenderSurface(const RenderSurface&);
			RenderSurface();
			~RenderSurface();
			//RenderSurface& operator=(const RenderSurface&);
		};

		bool depthPtrSortProc(RenderSurface* const &A, RenderSurface* const &B);
		bool materialDepthPtrSortProc(RenderSurface* const &A, RenderSurface* const &B);
		bool materialPtrSortProc(RenderSurface* const &A, RenderSurface* const &B);

		void sortByDepth(G3D::Array<RenderSurface*>& array);
		void sortByMaterial(G3D::Array<RenderSurface*>& array);
		void sortByMaterialAndDepth(G3D::Array<RenderSurface*>& array);
	}
}
