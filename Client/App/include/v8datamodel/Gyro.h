#pragma once
#include "util/RunStateOwner.h"
#include "v8kernel/Connector.h"

namespace RBX
{
	class World;
	class PartInstance;

	class BodyMover : public Instance, public Connector, public Listener<RunService, Stepped>
	{
	protected:
		World* world;
		PartInstance* part;

	protected:
		BodyMover(const char*);
	public:
		virtual ~BodyMover();
		virtual bool askSetParent(const Instance* instance) const;
	protected:
		virtual bool preventBodySleep() const { return true; }
		virtual void onAncestorChanged(const AncestorChanged& event);
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		virtual void onEvent(const RunService* source, Stepped event);
	private:
		void updateWorld();
	};

	extern const char* sBodyGyro;

	class BodyGyro : public DescribedCreatable<BodyGyro, BodyMover, &sBodyGyro>
	{
	private:
		float kP;
		float kD;
		G3D::Vector3 maxTorque;
		G3D::CoordinateFrame cframe;
	public:
		static Reflection::BoundProp<float, 1> prop_kP;
		static Reflection::BoundProp<float, 1> prop_kD;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_maxTorque;
		static Reflection::BoundProp<G3D::CoordinateFrame, 1> prop_cframe;

	private:
		void computeBalance(Body* body);
		void computeOrientation(Body* body);
	public:
		BodyGyro();
		virtual void computeForce(const float dt, bool throttling);
	};

	extern const char* sBodyPosition;

	class BodyPosition : public DescribedCreatable<BodyPosition, BodyMover, &sBodyPosition>
	{
	private:
		float kP;
		float kD;
		G3D::Vector3 maxForce;
		G3D::Vector3 position;
		G3D::Vector3 lastForce;
	public:
		static Reflection::BoundProp<float, 1> prop_kP;
		static Reflection::BoundProp<float, 1> prop_kD;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_maxForce;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_position;

	public:
		BodyPosition();
		G3D::Vector3 getLastForce() { return lastForce; }
		virtual void computeForce(const float dt, bool throttling);
	};

	extern const char* sBodyVelocity;

	class BodyVelocity : public DescribedCreatable<BodyVelocity, BodyMover, &sBodyVelocity>
	{
	private:
		float kP;
		G3D::Vector3 maxForce;
		G3D::Vector3 velocity;
		G3D::Vector3 lastForce;
	public:
		static Reflection::BoundProp<float, 1> prop_kP;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_maxForce;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_velocity;

	protected:
		virtual bool preventBodySleep() const { return velocity != G3D::Vector3::zero(); }
	public:
		BodyVelocity();
		G3D::Vector3 getLastForce();
		virtual void computeForce(const float dt, bool throttling);
	};

	extern const char* sBodyForce;

	class BodyForce : public DescribedCreatable<BodyForce, BodyMover, &sBodyForce>
	{
	private:
		G3D::Vector3 force;
	public:
		static Reflection::BoundProp<G3D::Vector3, 1> prop_force;

	protected:
		virtual bool preventBodySleep() const { return force != G3D::Vector3::zero(); }
	public:
		BodyForce();
		virtual void computeForce(const float dt, bool throttling);
	};

	extern const char* sBodyThrust;

	class BodyThrust : public DescribedCreatable<BodyThrust, BodyMover, &sBodyThrust>
	{
	private:
		G3D::Vector3 force;
		G3D::Vector3 location;
	public:
		static Reflection::BoundProp<G3D::Vector3, 1> prop_force;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_location;

	protected:
		virtual bool preventBodySleep() const { return force != G3D::Vector3::zero(); }
	public:
		BodyThrust();
		virtual void computeForce(const float dt, bool throttling);
	};

	extern const char* sRocket;

	class Rocket : public DescribedCreatable<Rocket, BodyMover, &sRocket>
	{
	private:
		bool active;
		boost::shared_ptr<PartInstance> target;
		G3D::Vector3 targetOffset;
		float targetRadius;
		bool firedEvent;
		float maxThrust;
		float kThrustP;
		float kThrustD;
		float maxSpeed;
		float kTurnP;
		float kTurnD;
		G3D::Vector3 maxTorque;
		float cartoonFactor;
	public:
		static Reflection::RefPropDescriptor<Rocket, PartInstance> prop_Target;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_targetOffset;
		static Reflection::BoundProp<float, 1> prop_targetRadius;
		static Reflection::BoundProp<float, 1> prop_MaxThrust;
		static Reflection::BoundProp<float, 1> prop_ThrustP;
		static Reflection::BoundProp<float, 1> prop_ThrustD;
		static Reflection::BoundProp<float, 1> prop_MaxSpeed;
		static Reflection::BoundProp<G3D::Vector3, 1> prop_MaxTorque;
		static Reflection::BoundProp<float, 1> prop_TurnP;
		static Reflection::BoundProp<float, 1> prop_TurnD;
		static Reflection::BoundProp<float, 1> prop_CartoonFactor;
		static Reflection::BoundFuncDesc<Rocket, void(void), 0> func_Fire;
		static Reflection::BoundFuncDesc<Rocket, void(void), 0> func_Abort;
		static Reflection::SignalDesc<Rocket, void(void)> event_ReachedTarget;

	public:
		Rocket();
		void fire();
		void abort();
		virtual void computeForce(const float dt, bool throttling);
	protected:
		virtual void onEvent(const RunService* source, Stepped event);
	private:
		G3D::Vector3 turn(Body* body, const G3D::Vector3& targetDir);
		PartInstance* getTarget() const { return target.get(); }
		void setTarget(PartInstance* value);
		void onGoalChanged(const Reflection::PropertyDescriptor&)
		{
			firedEvent = false;
		}
	};

	void registerBodyMovers();
}
