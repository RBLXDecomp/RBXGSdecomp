#include "RenderLib/RenderSurface.h"
#include <algorithm>

namespace RBX
{
	namespace Render
	{
		bool depthPtrSortProc(RenderSurface* const &A, RenderSurface* const &B)
		{
			return A->z < B->z;
		}

		bool materialDepthPtrSortProc(RenderSurface* const &A, RenderSurface* const &B)
		{
			return A->material < B->material || (A->material == B->material && A->z > B->z);
		}

		bool materialPtrSortProc(RenderSurface* const &A, RenderSurface* const &B)
		{
			return A->material < B->material;
		}

		void sortByDepth(G3D::Array<RenderSurface*>& array)
		{
			std::sort(array.begin(), array.end(), &depthPtrSortProc);
		}

		void sortByMaterial(G3D::Array<RenderSurface*>& array)
		{
			std::sort(array.begin(), array.end(), &materialPtrSortProc);
		}

		void sortByMaterialAndDepth(G3D::Array<RenderSurface*>& array)
		{
			std::sort(array.begin(), array.end(), &materialDepthPtrSortProc);
		}
	}
}
