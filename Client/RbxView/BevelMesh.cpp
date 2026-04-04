#include "BevelMesh.h"

namespace RBX
{
	namespace View
	{
		void BevelMesh::Builder::build(Purpose purpose)
		{
			LevelBuilder::build(purpose);
			if (bevel > 0.0f)
			{
				appendQuadFromIndexArray( 0,  3,  5,  4);
				appendQuadFromIndexArray( 4,  7,  8, 11);
				appendQuadFromIndexArray(11, 10,  1,  0);
				appendQuadFromIndexArray( 7,  6, 12, 15);
				appendQuadFromIndexArray(16, 19, 10,  9);
				appendQuadFromIndexArray( 3,  2, 21, 20);
				appendQuadFromIndexArray(19, 18,  2,  1);
				appendQuadFromIndexArray( 6,  5, 20, 23);
				appendQuadFromIndexArray(15, 14,  9,  8);
				appendQuadFromIndexArray(14, 13, 17, 16);
				appendQuadFromIndexArray(18, 17, 22, 21);
				appendQuadFromIndexArray(23, 22, 13, 12);
				appendQuadFromIndexArray( 0,  4, 11);
				appendQuadFromIndexArray( 5,  3, 20);
				appendQuadFromIndexArray( 1, 10, 19);
				appendQuadFromIndexArray( 2, 18, 21);
				appendQuadFromIndexArray(15,  8,  7);
				appendQuadFromIndexArray(12,  6, 23);
				appendQuadFromIndexArray(14, 16,  9);
				appendQuadFromIndexArray(13, 22, 17);
			}
		}
	}
}
