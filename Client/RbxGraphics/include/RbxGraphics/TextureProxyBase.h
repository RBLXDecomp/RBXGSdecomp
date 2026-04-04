#pragma once
#include <string>
#include <G3D/ReferenceCount.h>
#include <G3D/Vector2.h>

namespace RBX
{
	typedef G3D::ReferenceCountedPointer<class TextureProxyBase> TextureProxyBaseRef;

	class TextureProxyBase : public G3D::ReferenceCountedObject
	{
	public:
		std::string filename;
	  
	public:
		//TextureProxyBase(const TextureProxyBase&);
		TextureProxyBase()
		{
		}
		virtual ~TextureProxyBase()
		{
		}
	public:
		virtual G3D::Vector2 getSize() = 0;
	public:
		//TextureProxyBase& operator=(const TextureProxyBase&);
	};
}
