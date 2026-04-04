#include "QuadVolume.h"

namespace RBX
{
	namespace View
	{
		LevelBuilder::LevelBuilder(G3D::ReferenceCountedPointer<Render::Mesh::Level>& level, G3D::Vector3 size, RenderSurfaceTypes surfaceTypes)
			: halfSize(size * 0.5f),
			  surfaceTypes(surfaceTypes),
			  level(level),
			  textureScale(2.0f, 2.0f)
		{
		}

		float LevelBuilder::offset(RenderSurfaceType surfaceType)
		{
			switch (surfaceType)
			{
			case STUDS:
				return 0.0f;
			case INLET:
				return 0.5f;
			case WELD:
				return 1.0f;
			case GLUE:
				return 1.5f;

			default:
				return 0.0f;
			}
		}

		void LevelBuilder::build(Purpose purpose)
		{
			buildTop(purpose);
			buildBottom(purpose);
			buildLeft(purpose);
			buildRight(purpose);
			buildFront(purpose);
			buildBack(purpose);
		}

		void LevelBuilder::buildFace(NormalId normalId, Purpose purpose)
		{
			switch (normalId)
			{
			case NORM_X:
				buildRight(purpose);
				break;
			case NORM_Y:
				buildTop(purpose);
				break;
			case NORM_Z:
				buildBack(purpose);
				break;
			case NORM_X_NEG:
				buildLeft(purpose);
				break;
			case NORM_Y_NEG:
				buildBottom(purpose);
				break;
			case NORM_Z_NEG:
				buildFront(purpose);
				break;
			}
		}

		void LevelBuilder::appendQuadFromIndexArray(int a, int b, int c)
		{
			appendQuadFromVertexIndices(level->indexArray[a], level->indexArray[b], level->indexArray[c]);
		}

		void LevelBuilder::appendQuadFromIndexArray(int a, int b, int c, int d)
		{
			appendQuadFromVertexIndices(level->indexArray[a], level->indexArray[b], level->indexArray[c], level->indexArray[d]);
		}

		void LevelBuilder::appendQuadFromVertexIndices(unsigned a, unsigned b, unsigned c)
		{
			switch (level->primitive)
			{
			case G3D::RenderDevice::TRIANGLES:
				level->indexArray.append(
					Render::Mesh::allocVertex(a, 1),
					Render::Mesh::allocVertex(b, 1),
					Render::Mesh::allocVertex(c, 1)
				);
				break;
			case G3D::RenderDevice::QUADS:
				level->indexArray.append(
					Render::Mesh::allocVertex(a, 1),
					Render::Mesh::allocVertex(a, 1),
					Render::Mesh::allocVertex(b, 1),
					Render::Mesh::allocVertex(c, 1)
				);
				break;
			}
		}

		void LevelBuilder::appendQuadFromVertexIndices(unsigned a, unsigned b, unsigned c, unsigned d)
		{
			switch (level->primitive)
			{
			case G3D::RenderDevice::TRIANGLES:
				level->indexArray.append(
					Render::Mesh::allocVertex(a, 1),
					Render::Mesh::allocVertex(b, 1),
					Render::Mesh::allocVertex(c, 1)
				);
				level->indexArray.append(
					Render::Mesh::allocVertex(c, 1),
					Render::Mesh::allocVertex(d, 1),
					Render::Mesh::allocVertex(a, 1)
				);
				break;
			case G3D::RenderDevice::QUADS:
				level->indexArray.append(
					Render::Mesh::allocVertex(a, 1),
					Render::Mesh::allocVertex(b, 1),
					Render::Mesh::allocVertex(c, 1),
					Render::Mesh::allocVertex(d, 1)
				);
				break;
			}
		}
	}
}
