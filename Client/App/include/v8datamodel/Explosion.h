#pragma once
#include "util/RunStateOwner.h"
#include "util/IRenderable.h"

namespace RBX
{
	class Primitive;

	extern const char* sExplosion;

	class Explosion : public DescribedCreatable<Explosion, Instance, &sExplosion>,
					  public Listener<RunService, Stepped>,
					  public IRenderable
	{
	private:
		float age;
		G3D::Vector3 position;
		float blastRadius;
		float blastPressure;
	public:
		static Reflection::BoundProp<G3D::Vector3, 1> propPosition;
		static Reflection::BoundProp<float, 1> propBlastPressure;

	private:
		float renderTime() const;
		float visualRadius() const;
		float killRadius() const;
		float blastMaxObjectRadius() const;
		float killMaxObjectRadius() const;
		void doKill();
		void doBlast(const G3D::Array<Primitive*>& primitives);
		void signalBlast(const G3D::Array<Primitive*>& primitives);
		virtual void onEvent(const RunService*, Stepped);
		virtual bool askSetParent(const Instance* instance) const;
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);

		virtual bool shouldRender3dAdorn() const
		{
			return true;
		}

		virtual void render3dAdorn(Adorn* adorn);
	public:
		Explosion();

		virtual ~Explosion()
		{
		}

		void setVisualOnly();
		void setBlastRadius(float _blastRadius);

		float getBlastRadius() const
		{
			return blastRadius;
		}
	};
}
