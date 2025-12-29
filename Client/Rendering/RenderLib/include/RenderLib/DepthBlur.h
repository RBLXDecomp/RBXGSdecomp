#pragma once
#include <G3D/Rect2D.h>
#include <GLG3D/RenderDevice.h>
#include <GLG3D/Shader.h>
#include <GLG3D/Texture.h>

namespace RBX
{
	namespace Render
	{
		class DepthBlur
		{
		private:
			G3D::ReferenceCountedPointer<G3D::Shader> shader;
			G3D::ReferenceCountedPointer<G3D::Texture> depth;
			G3D::ReferenceCountedPointer<G3D::Texture> color;
		private:
			void allocateTextures(const G3D::Rect2D& screenRect);
		public:
			//DepthBlur(const DepthBlur&);
			DepthBlur();

			void apply(G3D::RenderDevice* rd);

			~DepthBlur();
			//DepthBlur& operator=(const DepthBlur&);
		};
	}
}
