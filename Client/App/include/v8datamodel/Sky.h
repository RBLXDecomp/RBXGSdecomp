#pragma once
#include "v8tree/Instance.h"
#include "util/TextureId.h"
#include <GLG3D/Sky.h>

namespace RBX
{
	extern const char* sSky;
	class Sky : public DescribedCreatable<Sky, Instance, &sSky>
	{
	public:
		TextureId skyUp;
		TextureId skyLf;
		TextureId skyRt;
		TextureId skyBk;
		TextureId skyFt;
		TextureId skyDn;
		bool drawCelestialBodies;
		int numStars;
	public:
		static Reflection::BoundProp<int, 1> prop_StarCount;
		static Reflection::BoundProp<TextureId, 1> prop_SkyUp;
		static Reflection::BoundProp<TextureId, 1> prop_SkyLf;
		static Reflection::BoundProp<TextureId, 1> prop_SkyRt;
		static Reflection::BoundProp<TextureId, 1> prop_SkyBk;
		static Reflection::BoundProp<TextureId, 1> prop_SkyFt;
		static Reflection::BoundProp<TextureId, 1> prop_SkyDn;
		static Reflection::BoundProp<bool, 1> prop_CelestialBodiesShown;

	public:
		//Sky(const Sky&);
		Sky();
		virtual ~Sky();
	public:
		//Sky& operator=(const Sky&);

	public:
		static G3D::ReferenceCountedPointer<G3D::Sky> getG3DSky(Sky*);
	};
}
