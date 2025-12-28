#include "RenderLib/DepthBlur.h"
#include <GLG3D/Draw.h>
#include <GLG3D/GLCaps.h>

namespace RBX
{
	namespace Render
	{
		DepthBlur::DepthBlur()
		{
			if (G3D::GLCaps::supports_GL_ARB_shader_objects() 
				&& G3D::GLCaps::supports_GL_ARB_shading_language_100() 
				&& G3D::GLCaps::supports_GL_EXT_texture_rectangle())
			{
				shader = G3D::Shader::fromStrings(
					"", 
					STR(
					uniform sampler2D depth; 
					uniform sampler2D color; 
					void main(void) { 
						float d = texture2D(depth, gl_TexCoord[0].xy).r; 
						float b = max(0.0, d - 0.99) * 100.0; 
						vec2 s = g3d_sampler2DInvSize(color) * b * 2.0; 
						vec3 c0 = texture2D(color, gl_TexCoord[0].xy + vec2( s.x, s.y)).rgb; 
						vec3 c1 = texture2D(color, gl_TexCoord[0].xy + vec2(-s.x, s.y)).rgb; 
						vec3 c2 = texture2D(color, gl_TexCoord[0].xy + vec2( s.x, -s.y)).rgb; 
						vec3 c3 = texture2D(color, gl_TexCoord[0].xy + vec2(-s.x, -s.y)).rgb; 
						vec3 c4 = texture2D(color, gl_TexCoord[0].xy + vec2( 0, s.y)).rgb; 
						vec3 c5 = texture2D(color, gl_TexCoord[0].xy + vec2(-s.x, 0)).rgb; 
						vec3 c6 = texture2D(color, gl_TexCoord[0].xy + vec2( 0, -s.y)).rgb; 
						vec3 c7 = texture2D(color, gl_TexCoord[0].xy + vec2( s.x, 0)).rgb; 
						vec3 c8 = texture2D(color, gl_TexCoord[0].xy).rgb; 
						gl_FragColor.rgb = (c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8) / 9.0; 
						gl_FragColor.a = 1.0; 
					}
					)
				);

				shader->setPreserveState(false);
			}
		}

		void DepthBlur::apply(G3D::RenderDevice* rd)
		{
			if (shader.notNull())
			{
				rd->push2D();
				G3D::Rect2D screenRect = rd->getViewport();

				allocateTextures(screenRect);
				depth->copyFromScreen(screenRect);
				color->copyFromScreen(screenRect);
				rd->setShader(shader);
			
				G3D::Draw::fastRect2D(screenRect, rd);
				rd->pop2D();
			}
		}

		void DepthBlur::allocateTextures(const G3D::Rect2D& screenRect)
		{
			if (depth.isNull() || depth->vector2Bounds() != screenRect.wh())
			{
				depth = G3D::Texture::createEmpty("Depth Buffer", screenRect.width(), screenRect.height(), G3D::TextureFormat::depth(0), G3D::Texture::DIM_2D_NPOT, G3D::Texture::Parameters::video());
				color = G3D::Texture::createEmpty("Color Buffer", screenRect.width(), screenRect.height(), G3D::TextureFormat::RGB8, G3D::Texture::DIM_2D_NPOT, G3D::Texture::Parameters::video());
				shader->args.set("depth", depth);
				shader->args.set("color", color);
			}
		}
	}
}
