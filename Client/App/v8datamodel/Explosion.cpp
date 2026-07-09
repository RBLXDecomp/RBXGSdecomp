#include "v8datamodel/Explosion.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/PartInstance.h"
#include "v8world/ContactManager.h"
#include "v8world/World.h"

namespace RBX
{
	static Reflection::PropDescriptor<Explosion, float> propBlastRadius("BlastRadius", "Data", &Explosion::getBlastRadius, &Explosion::setBlastRadius, Reflection::PropertyDescriptor::STANDARD);

	Reflection::SignalDesc<Explosion, void(boost::shared_ptr<Instance>, float)> signal_Hit("Hit", "part", "distance");

	Reflection::BoundProp<G3D::Vector3, 1> Explosion::propPosition("Position", "Data", &Explosion::position, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Explosion::propBlastPressure("BlastPressure", "Data", &Explosion::blastPressure, Reflection::PropertyDescriptor::STANDARD);

	Explosion::Explosion()
		: blastRadius(4.0f),
		  blastPressure(500000.0f),
		  age(0.0f)
	{
		setName("Explosion");
	}

	bool Explosion::askSetParent(const Instance* instance) const
	{
		return true;
	}

	void Explosion::setBlastRadius(float _blastRadius)
	{
		float newRadius = G3D::clamp(_blastRadius, 0.0f, 100.0f);
		if (blastRadius != newRadius)
		{
			blastRadius = newRadius;
			raisePropertyChanged(propBlastRadius);
		}
	}

	void Explosion::signalBlast(const G3D::Array<Primitive*>& primitives)
	{
		if (!signal_Hit.empty(this))
		{
			for (int i = 0; i < primitives.size(); i++)
			{
				Primitive* current = primitives[i];

				if (blastRadius * 2.0f > current->getRadius())
				{
					if (Instance* part = PartInstance::fromPrimitive(current))
					{
						float distance = (current->getCoordinateFrame().translation - position).magnitude();
						signal_Hit.fire(this, shared_from(part), distance);
					}
				}
			}
		}
	}

	//99% match
	void Explosion::doBlast(const G3D::Array<Primitive*>& primitives)
	{
		if (blastPressure > 0.0f)
		{
			World* world = Workspace::getMyWorldFast(this);
			RBXASSERT(world);

			for (int i = 0; i < primitives.size(); i++)
			{
				Primitive* current = primitives[i];

				if (blastRadius * 2.0f > current->getRadius())
				{
					PartInstance::fromPrimitive(current)->destroyJoints();
				}
			}

			world->update();

			for (int i = 0; i < primitives.size(); i++)
			{
				Primitive* current = primitives[i];

				if (blastRadius * 2.0f > current->getRadius())
				{
					G3D::Vector3 delta = current->getCoordinateFrame().translation - position;
					G3D::Vector3 normal = delta == G3D::Vector3::zero() ? G3D::Vector3::unitY() : delta.direction();
					RBXASSERT(normal.magnitude() <= 1.001);

					float radius = current->getRadius();
					float squaredRadius = radius * radius;

					G3D::Vector3 impulse = normal * blastPressure * squaredRadius * (1.0f/4560.0f);
					G3D::Vector3 force = impulse * Constants::kernelStepsPerSec();

					world->ticklePrimitive(current);

					Body* body = current->getBody();

					const G3D::Vector3& pos = body->getCoordinateFrame().translation;
					body->accumulateForce(force, pos);

					current->getBody()->accumulateTorque(force * 0.5f * radius);

					world->addedBodyForce();
				}
			}
		}
	}

	void Explosion::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Listener<RunService, Stepped>* oldListener = this;

		if (oldProvider)
		{
			if (RunService* runService = oldProvider->find<RunService>())
				runService->Notifier<RunService, Stepped>::removeListener(oldListener);
		}

		Instance::onServiceProvider(oldProvider, newProvider);

		Listener<RunService, Stepped>* newListener = this;

		if (newProvider)
		{
			if (RunService* runService = newProvider->find<RunService>())
				runService->Notifier<RunService, Stepped>::addListener(newListener);
		}
	}
}
