#include "v8datamodel/Gyro.h"
#include "v8datamodel/PartInstance.h"
#include <Network/Players.h>

namespace RBX
{

	const char* sBodyPosition = "BodyPosition";
	const char* sBodyVelocity = "BodyVelocity";
	const char* sBodyGyro = "BodyGyro";
	const char* sBodyForce = "BodyForce";
	const char* sBodyThrust = "BodyThrust";
	const char* sRocket = "Rocket";
	void registerBodyMovers()
	{
		BodyGyro::classDescriptor();
		BodyPosition::classDescriptor();
		BodyVelocity::classDescriptor();
		BodyForce::classDescriptor();
		BodyThrust::classDescriptor();
		Rocket::classDescriptor();
	}

	BodyMover::BodyMover(const char* name)
		: Instance(name),
		  world(NULL),
		  part(NULL)
	{
	}

	BodyMover::~BodyMover()
	{
		RBXASSERT(world == NULL);
		RBXASSERT(part == NULL);
	}

	void BodyMover::updateWorld()
	{
		World* worldIfInWorkspace = (part != NULL) ? Workspace::getWorldIfInWorkspace(this) : NULL;
		if (world != worldIfInWorkspace)
		{
			if (world)
				world->getKernel().removeConnector2ndPass(this);
			world = worldIfInWorkspace;
			if (worldIfInWorkspace)
			{
				worldIfInWorkspace->getKernel().insertConnector2ndPass(this);
				Notifier<RunService,Stepped>::connect<RunService*>(ServiceProvider::create<RunService>(this), this);
			}
		}
	}

	void BodyMover::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Notifier<RunService,Stepped>::disconnect<RunService*>(ServiceProvider::create<RunService>(oldProvider), this);

		Instance::onServiceProvider(oldProvider, newProvider);

		updateWorld();
	}

	void BodyMover::onEvent(const RunService* source, Stepped event)
	{
		if (preventBodySleep())
		{
			world->ticklePrimitive(part->getPrimitive());
		}
	}

	void BodyMover::onAncestorChanged(const AncestorChanged& event)
	{
		Instance::onAncestorChanged(event);

		if (event.child == this)
			part = fastDynamicCast<PartInstance>(event.newParent);
		updateWorld();
	}

	bool BodyMover::askSetParent(const Instance* instance) const
	{
		return fastDynamicCast<const PartInstance>(instance) != NULL;
	}

	Reflection::RefPropDescriptor<Rocket, PartInstance> Rocket::prop_Target("Target", "Goals", &Rocket::getTarget, &Rocket::setTarget, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> Rocket::prop_targetOffset("TargetOffset", "Goals", &Rocket::targetOffset, &Rocket::onGoalChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_targetRadius("TargetRadius", "Goals", &Rocket::targetRadius, &Rocket::onGoalChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_MaxSpeed("MaxSpeed", "Thrust", &Rocket::maxSpeed, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_MaxThrust("MaxThrust", "Thrust", &Rocket::maxThrust, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_ThrustP("ThrustP", "Thrust", &Rocket::kThrustP, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_ThrustD("ThrustD", "Thrust", &Rocket::kThrustD, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_TurnP("TurnP", "Turn", &Rocket::kTurnP, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_TurnD("TurnD", "Turn", &Rocket::kTurnD, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> Rocket::prop_MaxTorque("MaxTorque", "Turn", &Rocket::maxTorque, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> Rocket::prop_CartoonFactor("CartoonFactor", "Goals", &Rocket::cartoonFactor, Reflection::PropertyDescriptor::STANDARD);

	Reflection::BoundFuncDesc<Rocket, void(void), 0> Rocket::func_Fire(&Rocket::fire, "Fire", Reflection::FunctionDescriptor::AnyCaller);
	Reflection::BoundFuncDesc<Rocket, void(void), 0> Rocket::func_Abort(&Rocket::abort, "Abort", Reflection::FunctionDescriptor::AnyCaller);

	Reflection::SignalDesc<Rocket, void(void)> Rocket::event_ReachedTarget("ReachedTarget");

	Rocket::Rocket()
		: active(false),
		  targetOffset(),
		  target(),
		  firedEvent(false),
		  targetRadius(4.0f),
		  maxThrust(4000.0f),
		  kThrustP(5.0f),
		  kThrustD(0.001f),
		  maxSpeed(30.0f),
		  kTurnP(3000.0f),
		  kTurnD(500.0f),
		  maxTorque(G3D::Vector3(400000.0f, 400000.0f, 0.0f)),
		  cartoonFactor(0.7f)
	{
	}

	void Rocket::onEvent(const RunService* source, Stepped event)
	{
		if (preventBodySleep()) world->ticklePrimitive(part->getPrimitive());

		if (active && !firedEvent && part)
		{
			G3D::Vector3 pos = part->getCoordinateFrame().translation;
			G3D::Vector3 targetWorld = target.get() ? target->getCoordinateFrame().pointToWorldSpace(targetOffset) : targetOffset;
			pos = pos - targetWorld;
			if (pos.squaredMagnitude() <= targetRadius * targetRadius)
			{
				Rocket::event_ReachedTarget.fire(this);
				firedEvent = true;
			}
		}
	}

	void Rocket::fire()
	{
		if (!Workspace::findWorkspace(this))
			throw std::runtime_error("Rocket:Fire may only be called when the Rocket is in the Workspace");
		if (Network::Players::clientIsPresent(this, true))
			throw std::runtime_error("Rocket:Fire may not be called from a client");
		active = true;
	}

	void Rocket::abort()
	{
		active = false;
	}

	void Rocket::setTarget(PartInstance* value)
	{
		if (target.get() != value)
		{
			target = shared_from<PartInstance>(value);
			firedEvent = false;
			raisePropertyChanged(Rocket::prop_Target);
		}
	}
	


	void Rocket::computeForce(const float dt, bool throttling)
	{
		if (!active)
			return;

		Body* body = part->getPrimitive()->getBody();
		RBXASSERT(body != NULL);

		G3D::Vector3 targetPos = target.get() ? target->getCoordinateFrame().pointToWorldSpace(targetOffset) : targetOffset;
		G3D::Vector3 targetDelta = targetPos - body->getCoordinateFrame().translation; // fabricated name, may change
		G3D::Vector3 targetDir = targetDelta.direction();
		G3D::Vector3 pAccel = targetDelta * kThrustP;
		Body* root = body->getRoot();

		G3D::Vector3 force = pAccel * root->getBranchMass();

		if (root->getVelocity().linear.squaredMagnitude() > maxSpeed * maxSpeed)
		{
			G3D::Vector3 dir = root->getVelocity().linear.direction();
			float dotProduct = dir.dot(force);
			if (dotProduct > 0.0f)
			{
				force -= dir * dotProduct;
				pAccel = dir * maxSpeed;
				G3D::Vector3 low = root->getVelocity().linear - pAccel;
				force -= (low * root->getBranchMass()) / dt;
			}
		}

		G3D::Vector3 adjustedVelocity = (root->getBranchMass() * kThrustD) * root->getVelocity().linear; // fabricated name, may change
		force -= adjustedVelocity;

		force -= body->getBranchForce();

		Math::fixDenorm(force);

		force = force.clamp(G3D::Vector3(-maxThrust, -maxThrust, -maxThrust), G3D::Vector3(maxThrust, maxThrust, maxThrust));

		root->accumulateForceAtBranchCofm(force);

		G3D::Vector3 unitForce = force.squaredMagnitude() > 0.000001f ? force.fastDirection() : G3D::Vector3::zero(); // fabricated name, may change
		G3D::Vector3 forceCartoon = unitForce * (1.0f - cartoonFactor); // fabricated name, may change
		G3D::Vector3 newDirection = forceCartoon + (targetDir * cartoonFactor); // fabricated name, may change
		turn(body, newDirection.direction());
	}

	G3D::Vector3 Rocket::turn(Body* body, const G3D::Vector3& targetDir)
	{
		G3D::Vector3 targetObjectSpace = body->getCoordinateFrame().vectorToObjectSpace(targetDir);

		Body* root = body->getRoot();
		G3D::Vector3 oldTorqueWorld = root->getBranchTorque();

		float desiredTorqueX = idk.y * root->getBranchIBodyV3().x * kTurnP;
		float desiredTorqueY = -(idk.x * root->getBranchIBodyV3().y * kTurnP);

		G3D::Vector3 torqueBody = body->getCoordinateFrame().vectorToObjectSpace(oldTorqueWorld);
		
		if (fabs(desiredTorqueX - torqueBody.x) < root->getBranchIBodyV3().x * maxTorque.x)
			torqueBody.x = desiredTorqueX;
		else
			torqueBody.x += desiredTorqueX;
		
		if (fabs(desiredTorqueY - torqueBody.y) < root->getBranchIBodyV3().y * maxTorque.y)
			torqueBody.y = desiredTorqueY;
		else
			torqueBody.y += desiredTorqueY;

		G3D::Vector3 angVelBody = body->getCoordinateFrame().vectorToObjectSpace(body->getVelocity().rotational);
		torqueBody.x -= (root->getBranchIBodyV3().x * angVelBody.x) * kTurnD;
		torqueBody.y -= (root->getBranchIBodyV3().y * angVelBody.y) * kTurnD;

		G3D::Vector3 addedTorque = body->getCoordinateFrame().vectorToWorldSpace(torqueBody) - oldTorqueWorld;
		root->accumulateTorque(addedTorque);

		return targetObjectSpace;
	}


	Reflection::BoundProp<float, 1> BodyGyro::prop_kP("P", "Goals", &BodyGyro::kP, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> BodyGyro::prop_kD("D", "Goals", &BodyGyro::kD, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyGyro::prop_maxTorque("maxTorque", "Goals", &BodyGyro::maxTorque, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::CoordinateFrame, 1> BodyGyro::prop_cframe("cframe", "Goals", &BodyGyro::cframe, Reflection::PropertyDescriptor::STANDARD);

	BodyGyro::BodyGyro()
		: kP(3000.0f),
		  kD(500.0f),
		  maxTorque(G3D::Vector3(400000.0f, 0.0f, 400000.0f)),
		  cframe()
	{
	}

	void BodyGyro::computeForce(const float dt, bool throttling)
	{
		Body* body = part->getPrimitive()->getBody();
		RBXASSERT(body != NULL);

		computeBalance(body);
		computeOrientation(body);
	}

	void BodyGyro::computeOrientation(Body* body)
	{
		if (G3D::fuzzyEq(maxTorque.y, 0.0f))
			return;

		Body* root = body->getRoot();

		G3D::Vector3 oldTorqueWorld = root->getBranchTorque();


		G3D::Vector3 localZNegAxis = body->getCoordinateFrame().vectorToObjectSpace(cframe.lookVector());


		float desiredTorqueY = -(root->getBranchIBodyV3().y * localZNegAxis.x * kP);

		G3D::Vector3 torqueBody = body->getCoordinateFrame().vectorToObjectSpace(oldTorqueWorld);

		if (fabs(desiredTorqueY - torqueBody.y) < root->getBranchIBodyV3().y * maxTorque.y)
			torqueBody.y = desiredTorqueY;
		else
			torqueBody.y = torqueBody.y + desiredTorqueY;


		G3D::Vector3 angVelBody = body->getCoordinateFrame().vectorToObjectSpace(body->getVelocity().rotational);
		torqueBody.y = torqueBody.y - kD * root->getBranchIBodyV3().y * angVelBody.y;

		G3D::Vector3 addedTorque = body->getCoordinateFrame().vectorToWorldSpace(torqueBody) - oldTorqueWorld;

		root->accumulateTorque(addedTorque);
	}



	void BodyGyro::computeBalance(Body* body)
	{
		if (G3D::fuzzyEq(maxTorque.x, 0.0f) && G3D::fuzzyEq(maxTorque.z, 0.0f))
			return;

		Body* root = body->getRoot();
		G3D::Vector3 oldTorqueWorld = root->getBranchTorque();
		G3D::Vector3 localYAxis = body->getCoordinateFrame().vectorToObjectSpace(cframe.upVector());
		float desiredTorqueX = root->getBranchIBodyV3().x * localYAxis.z * kP;
		float desiredTorqueZ = -(root->getBranchIBodyV3().z * localYAxis.x * kP);

		G3D::Vector3 torqueBody = body->getCoordinateFrame().vectorToObjectSpace(oldTorqueWorld);

		if (fabs(desiredTorqueX - torqueBody.x) < root->getBranchIBodyV3().x * maxTorque.x)
			torqueBody.x = desiredTorqueX;
		else
			torqueBody.x += desiredTorqueX;

		if (fabs(desiredTorqueZ - torqueBody.z) < root->getBranchIBodyV3().z * maxTorque.z)
			torqueBody.z = desiredTorqueZ;
		else
			torqueBody.z += desiredTorqueZ;

		G3D::Vector3 angVelBody = body->getCoordinateFrame().vectorToObjectSpace(body->getVelocity().rotational);
		torqueBody.x -= (root->getBranchIBodyV3().x * angVelBody.x) * kD;
		torqueBody.z -= (root->getBranchIBodyV3().z * angVelBody.z) * kD;

		G3D::Vector3 addedTorque = body->getCoordinateFrame().vectorToWorldSpace(torqueBody) - oldTorqueWorld;

		root->accumulateTorque(addedTorque);
	}

	Reflection::BoundProp<float, 1> BodyPosition::prop_kP("P", "Goals", &BodyPosition::kP, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> BodyPosition::prop_kD("D", "Goals", &BodyPosition::kD, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyPosition::prop_maxForce("maxForce", "Goals", &BodyPosition::maxForce, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyPosition::prop_position("position", "Goals", &BodyPosition::position, Reflection::PropertyDescriptor::STANDARD);



	static Reflection::BoundFuncDesc<BodyPosition, G3D::Vector3(void), 0> func_getLastForceOld(&BodyPosition::getLastForce, "lastForce", Reflection::FunctionDescriptor::AnyCaller);
	static Reflection::BoundFuncDesc<BodyPosition, G3D::Vector3(void), 0> func_getLastForce(&BodyPosition::getLastForce, "GetLastForce", Reflection::FunctionDescriptor::AnyCaller);

	BodyPosition::BodyPosition()
		: kP(10000.0f),
		  kD(1250.0f),
		  maxForce(G3D::Vector3(4000.0f, 4000.0f, 4000.0f)),
		  position(G3D::Vector3(0.0f, 50.0f, 0.0f)),
		  lastForce()
	{
	}



	void BodyPosition::computeForce(const float dt, bool throttling)
	{
		RBXASSERT(part->getPrimitive()->getBody() != NULL);
		Body* body = part->getPrimitive()->getBody();
		
		G3D::Vector3 pAccel = (position - body->getCoordinateFrame().translation) * kP;
		pAccel = pAccel + (body->getVelocity().linear * -kD);
		//Body* root = body->getRoot();
		lastForce = pAccel * body->getRoot()->getBranchMass();
		lastForce = lastForce.clamp(-maxForce, maxForce);
		
		body->getRoot()->accumulateForce(lastForce, body->getRoot()->getCoordinateFrame().translation);
	}

	Reflection::BoundProp<float, 1> BodyVelocity::prop_kP("P", "Goals", &BodyVelocity::kP, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyVelocity::prop_maxForce("maxForce", "Goals", &BodyVelocity::maxForce, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyVelocity::prop_velocity("velocity", "Goals", &BodyVelocity::velocity, Reflection::PropertyDescriptor::STANDARD);



	static Reflection::BoundFuncDesc<BodyVelocity, G3D::Vector3(void), 0> func_getLastForceVOld(&BodyVelocity::getLastForce, "lastForce", Reflection::FunctionDescriptor::AnyCaller);
	static Reflection::BoundFuncDesc<BodyVelocity, G3D::Vector3(void), 0> func_getLastForceV(&BodyVelocity::getLastForce, "GetLastForce", Reflection::FunctionDescriptor::AnyCaller);

	BodyVelocity::BodyVelocity()
		: kP(1250.0f),
		  maxForce(G3D::Vector3(4000.0f, 4000.0f, 4000.0f)),
		  velocity(G3D::Vector3(0.0f, 2.0f, 0.0f)),
		  lastForce()
	{
	}


	void BodyVelocity::computeForce(const float dt, bool throttling)
	{
		RBXASSERT(part->getPrimitive()->getBody() != NULL);
		Body* body = part->getPrimitive()->getBody();
		//Body* root = part->getPrimitive()->getBody()->getRoot();

		G3D::Vector3 pAccel = (velocity - body->getVelocity().linear) * kP;

		lastForce = pAccel * body->getRoot()->getBranchMass();
		lastForce = lastForce.clamp(-maxForce, maxForce);

		body->getRoot()->accumulateForce(lastForce, body->getRoot()->getCoordinateFrame().translation);
	}



	Reflection::BoundProp<G3D::Vector3, 1> BodyForce::prop_Force("force", "Goals", &BodyForce::force, Reflection::PropertyDescriptor::STANDARD);


	BodyForce::BodyForce()
		: force(G3D::Vector3::unitY())
	{
	}

	void BodyForce::computeForce(const float dt, bool throttling)
	{
		RBXASSERT(part->getPrimitive()->getBody() != NULL);
		Body* root = part->getPrimitive()->getBody()->getRoot();

		root->accumulateForce(force, root->getCoordinateFrame().translation);
	}

	Reflection::BoundProp<G3D::Vector3, 1> BodyThrust::prop_force("force", "Goals", &BodyThrust::force, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<G3D::Vector3, 1> BodyThrust::prop_location("location", "Goals", &BodyThrust::location, Reflection::PropertyDescriptor::STANDARD);


	BodyThrust::BodyThrust()
		: force(G3D::Vector3::unitY()),
		  location(G3D::Vector3::zero())
	{
	}

	void BodyThrust::computeForce(const float dt, bool throttling)
	{
		Body* body = part->getPrimitive()->getBody();
		RBXASSERT(body != NULL);

		G3D::Vector3 worldForce = body->getCoordinateFrame().vectorToWorldSpace(force);
		G3D::Vector3 worldPos = body->getCoordinateFrame().pointToWorldSpace(location);
		body->accumulateForce(worldForce, worldPos);
	}
}
