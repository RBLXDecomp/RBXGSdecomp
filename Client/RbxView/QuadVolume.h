#pragma once
#include "RenderLib/Mesh.h"
#include "util/NormalId.h"

namespace RBX
{
	namespace View
	{
		enum RenderSurfaceType
		{
			FLAT,
			STUDS,
			INLET,
			WELD,
			GLUE
		};

		class RenderSurfaceTypes // NOTE: may not be intended for this file
		{
		public:
			size_t data;
		public:
			RenderSurfaceTypes()
				: data(0)
			{
			}
			void setSurfaceType(NormalId, RenderSurfaceType);
			RenderSurfaceType getSurfaceType(NormalId) const;
			unsigned int hashCode() const;

			bool operator==(const RenderSurfaceTypes&) const;
			bool operator<(const RenderSurfaceTypes&) const;
			bool operator>(const RenderSurfaceTypes&) const;
		};

		class __declspec(novtable) LevelBuilder
		{
		public:
			enum Purpose
			{
				Surface,
				Decal,
				Shadow
			};
		protected:
			const G3D::Vector3 halfSize;
			const RenderSurfaceTypes surfaceTypes;
			const G3D::ReferenceCountedPointer<Render::Mesh::Level> level;
		public:
			G3D::Vector2 textureScale;
			
		protected:
			LevelBuilder(G3D::ReferenceCountedPointer<Render::Mesh::Level>& level, G3D::Vector3 size, RenderSurfaceTypes surfaceTypes);
		public:
			virtual void build(Purpose purpose);
			void buildFace(NormalId normalId, Purpose purpose);

			virtual void buildTop(Purpose purpose) = 0;
			virtual void buildBottom(Purpose purpose) = 0;
			virtual void buildLeft(Purpose purpose) = 0;
			virtual void buildRight(Purpose purpose) = 0;
			virtual void buildFront(Purpose purpose) = 0;
			virtual void buildBack(Purpose purpose) = 0;
		protected:
			void appendQuadFromIndexArray(int a, int b, int c, int d);
			void appendQuadFromIndexArray(int a, int b, int c);
			void appendQuadFromVertexIndices(unsigned a, unsigned b, unsigned c, unsigned d);
			void appendQuadFromVertexIndices(unsigned a, unsigned b, unsigned c);

			template<NormalId id, class Function>
			void buildFace(Function, G3D::Rect2D, const float, G3D::Vector2, G3D::Vector2, const int, G3D::Vector2int16);

			template<NormalId id, class Function>
			void buildFace(Function, G3D::Vector2int16, Purpose);

			template<NormalId id, class Builder>
			void buildFace(Builder, const unsigned, const unsigned, Purpose);

		protected:
			static float offset(RenderSurfaceType surfaceType);
		};
	}
}
