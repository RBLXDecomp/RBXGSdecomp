#include "humanoid/Humanoid.h"
#include "humanoid/Running.h"
#include "humanoid/FallingDown.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/JointInstance.h"
#include "v8datamodel/ModelInstance.h"
#include "v8datamodel/Workspace.h"
#include "v8world/World.h"
#include "Network/Players.h"
#include "AppDraw/DrawAdorn.h"

namespace RBX
{
	Reflection::PropDescriptor<Humanoid, G3D::Vector3> propWalkToPoint("WalkToPoint", "Control", &Humanoid::getWalkToPoint, &Humanoid::setWalkToPoint, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Humanoid, G3D::Vector3> propWalkDirection("WalkDirection", "Control", &Humanoid::getWalkDirection, &Humanoid::setWalkDirection, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Humanoid, float> propWalkRotationalVelocity("WalkRotationalVelocity", "Control", &Humanoid::getWalkRotationalVelocity, &Humanoid::setWalkRotationalVelocity, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Humanoid, bool> propJump("Jump", "Control", &Humanoid::getJump, &Humanoid::setJump, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Humanoid, G3D::Vector3> propTargetPoint("TargetPoint", "Control", &Humanoid::getTargetPoint, &Humanoid::setTargetPoint, Reflection::PropertyDescriptor::STREAMING);
	Reflection::PropDescriptor<Humanoid, bool> propSit("Sit", "Control", &Humanoid::getSit, &Humanoid::setSit, Reflection::PropertyDescriptor::STREAMING);

	Reflection::BoundFuncDesc<Humanoid, void(G3D::Vector3, boost::shared_ptr<Instance>), 2> func_MoveTo(&Humanoid::moveTo2, "MoveTo", "location", "part", Reflection::FunctionDescriptor::AnyCaller);

	Reflection::RefPropDescriptor<Humanoid, PartInstance> propTorso("Torso", "Data", &Humanoid::getTorso, &Humanoid::setTorso, Reflection::PropertyDescriptor::STANDARD);
	Reflection::RefPropDescriptor<Humanoid, PartInstance> propLeftLeg("LeftLeg", "Data", &Humanoid::getLeftLeg, &Humanoid::setLeftLeg, Reflection::PropertyDescriptor::STANDARD);
	Reflection::RefPropDescriptor<Humanoid, PartInstance> propRightLeg("RightLeg", "Data", &Humanoid::getRightLeg, &Humanoid::setRightLeg, Reflection::PropertyDescriptor::STANDARD);
	Reflection::RefPropDescriptor<Humanoid, PartInstance> propWalkToPart("WalkToPart", "Control", &Humanoid::getWalkToPart, &Humanoid::setWalkToPart, Reflection::PropertyDescriptor::STREAMING);

	Humanoid::Humanoid()
		: health(100.0f),
		  maxHealth(100.0f),
		  walkMode(DIRECTION_MOVE),
		  walkTimer(0),
		  walkRotationalVelocity(0.0f),
		  jump(false),
		  imDead(false),
		  hadHeadJoint(false),
		  sit(false),
		  world(NULL)
	{
		setName("Humanoid");
	}

	Humanoid::~Humanoid() 
	{
		RBXASSERT(!world);
	}

	PartInstance* Humanoid::getHeadFromCharacter(const ModelInstance* character)
	{
		return character ? dynamic_cast<PartInstance*>(character->findFirstChildByName("Head")) : NULL;
	}

	G3D::CoordinateFrame Humanoid::getTopOfHead() const
	{
		return G3D::CoordinateFrame(G3D::Vector3(0.0f, 0.5f, 0.0f));
	}

	void Humanoid::renderWaypoint(Adorn* adorn, const G3D::Vector3& waypoint)
	{
		DrawAdorn::cylinder(adorn, G3D::CoordinateFrame(waypoint), 1, 0.2f, 1.0f, G3D::Color3::green());
	}

	ContactManager* Humanoid::getContactManager()
	{
		World* world = Workspace::getWorldIfInWorkspace(this);
		return world ? &world->getContactManager() : NULL;
	}

	PartInstance* Humanoid::getLocalHeadFromContext(const Instance* context)
	{
		return getHeadFromCharacter(Network::Players::findLocalCharacter(context));
	}

	bool Humanoid::hasWalkToPoint(G3D::Vector3& worldPosition) const
	{
		if (walkToPart && Workspace::findWorkspace(this) && Workspace::contextInWorkspace(walkToPart.get()))
		{
			worldPosition = walkToPart->getCoordinateFrame().pointToWorldSpace(walkToPoint);
			return true;
		}
		else
		{
			return false;
		}
	}

	G3D::CoordinateFrame Humanoid::getRightArmGrip() const
	{
		G3D::CoordinateFrame cframe;
		cframe.lookAt(G3D::Vector3(0.0f, -1.0f, 0.0f), G3D::Vector3(0.0f, 0.0f, -1.0f));
		cframe.translation = G3D::Vector3(0.0f, -1.0f, 0.0f);

		return cframe;
	}

	PartInstance* Humanoid::findRightArm() const
	{
		return getParent() ? rbx_static_cast<PartInstance*>(getParent()->findFirstChildByName("Right Arm")) : NULL;
	}

	JointInstance* Humanoid::findRightShoulder() const
	{
		PartInstance* torso = getTorso();
		return torso ? rbx_static_cast<JointInstance*>(torso->findFirstChildByName("Right Shoulder")) : NULL;
	}

	void Humanoid::computeForce(const float dt, bool throttling)
	{
		if (currentState.get())
			currentState->onComputeForce(dt);
	}

	void Humanoid::onAncestorChanged(const AncestorChanged& event)
	{
		World* worldInWorkspace = Workspace::getWorldIfInWorkspace(this);
		if (worldInWorkspace != world)
		{
			currentState.reset();
			if (world)
			{
				world->getKernel().removeConnector2ndPass(this);
			}

			world = worldInWorkspace;
			if (world)
			{
				Humanoid::State* running = new Running(this);
				if (running != currentState.get())
					currentState.reset(running);

				world->getKernel().insertConnector2ndPass(this);
			}
		}

		Instance::onAncestorChanged(event);
	}

	float Humanoid::getIntendedRotationAboutYAxis()
	{
		return currentState.get() ? currentState->getIntendedRotationAboutYAxis() : 0.0f;
	}

	Humanoid* Humanoid::modelIsCharacter(const Instance* testModel)
	{
		return testModel ? testModel->findFirstChildOfType<Humanoid>() : NULL;
	}

	Humanoid* Humanoid::getLocalHumanoidFromContext(const Instance* context)
	{
		ModelInstance* character = Network::Players::findLocalCharacter(context);
		return character ? character->findFirstChildOfType<Humanoid>() : NULL;
	}

	void Humanoid::render3dAdorn(Adorn* adorn)
	{
		if (walkMode == CLICK_TO_MOVE)
		{
			if (this == modelIsCharacter(Network::Players::findLocalCharacter(this)) && Workspace::findWorkspace(this))
			{
				G3D::Vector3 worldPosition;
				if (hasWalkToPoint(worldPosition))
					renderWaypoint(adorn, worldPosition);
			}
		}
	}

	PartInstance* Humanoid::getHead() const
	{
		if (!head && getParent())
			head = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Head"));

		return head.get();
	}

	PartInstance* Humanoid::getTorso() const
	{
		if (!torso && getParent())
			torso = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Torso"));

		return torso.get();
	}

	PartInstance* Humanoid::getLeftLeg() const
	{
		if (!leftLeg && getParent())
			leftLeg = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Left Leg"));

		return leftLeg.get();
	}

	PartInstance* Humanoid::getRightLeg() const
	{
		if (!rightLeg && getParent())
			rightLeg = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Right Leg"));

		return rightLeg.get();
	}

	PartInstance* Humanoid::getLeftArm() const
	{
		if (!leftArm && getParent())
			leftArm = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Left Arm"));

		return leftArm.get();
	}

	PartInstance* Humanoid::getRightArm() const
	{
		if (!rightArm && getParent())
			rightArm = shared_from_dynamic_cast<PartInstance>(getParent()->findFirstChildByName("Right Arm"));

		return rightArm.get();
	}

	Primitive* Humanoid::getHeadPrimitive()
	{
		PartInstance* part = getHead();
		return part ? part->getPrimitive() : NULL;
	}

	Primitive* Humanoid::getTorsoPrimitive() const
	{
		PartInstance* part = getTorso();
		return part ? part->getPrimitive() : NULL;
	}

	Primitive* Humanoid::getLeftLegPrimitive()
	{
		PartInstance* part = getLeftLeg();
		return part ? part->getPrimitive() : NULL;
	}

	Primitive* Humanoid::getRightLegPrimitive()
	{
		PartInstance* part = getRightLeg();
		return part ? part->getPrimitive() : NULL;
	}

	Body* Humanoid::getTorsoBody()
	{
		PartInstance* part = getTorso();
		return part ? part->getPrimitive()->getBody() : NULL;
	}

	Body* Humanoid::getRootBody()
	{
		Body* body = getTorsoBody();
		return body ? body->getRoot() : NULL;
	}

	float Humanoid::getTorsoHeading() const
	{
		if (Primitive* primitive = getTorsoPrimitive())
		{
			return Math::getHeading(primitive->getCoordinateFrame().lookVector());
		}
		else
		{
			return 0.0f;
		}
	}

	void Humanoid::checkForJointDeath()
	{
		if (Primitive* headPrimitive = getHeadPrimitive())
		{
			if (Primitive* torsoPrimitive = getTorsoPrimitive())
			{
				for (Joint* current = headPrimitive->getFirstJoint(); current != NULL; current = headPrimitive->getNextJoint(current))
				{
					if (current->otherPrimitive(headPrimitive) == torsoPrimitive)
					{
						hadHeadJoint = true;
						return;
					}
				}
			}
		}

		if (hadHeadJoint)
		{
			propHealth.setValue(this, 0.0f);
		}
	}

	void Humanoid::onEvent(const RunService* source, Stepped event)
	{
		if (currentState.get())
		{
			if (getParent())
			{
				State* prevState = currentState.get();
				State* newState = prevState->onStep(event.step, *static_cast<PVInstance*>(getParent())->getTopPVController());

				if (newState != currentState.get())
					currentState.reset(newState);
			}

			if (getTorsoPrimitive())
			{
				getTorsoPrimitive()->setCanSleep(currentState->canSleep());
			}
		}

		checkForJointDeath();

		if (walkTimer > 0)
		{
			walkTimer--;
			if (walkTimer == 0)
				walkMode = DIRECTION_MOVE;
		}
	}

	const G3D::CoordinateFrame Humanoid::getLocation() const
	{
		return getHead() ? getHead()->getCoordinateFrame() : G3D::CoordinateFrame();
	}

	G3D::Vector3 Humanoid::getIntendedMovementVector()
	{
		return getIntendedMovementVector(false);
	}

	G3D::Vector3 Humanoid::updateWalkDirection()
	{
		return getIntendedMovementVector(true);
	}

	void Humanoid::onLocalHumanoidEnteringWorkspace()
	{
		if (Workspace* workspace = ServiceProvider::find<Workspace>(this))
		{
			if (this == modelIsCharacter(Network::Players::findLocalCharacter(this)))
			{
				if (workspace->getCamera()->getCameraType() == Camera::CUSTOM_CAMERA && workspace->getCamera()->getCameraSubject() != this)
				{
					workspace->getCamera()->setCameraSubject(this);
				}

				if (getHead())
					getHead()->setRenderImportance(1.0f);

				if (getLeftLeg())
					getLeftLeg()->setRenderImportance(1.0f);

				if (getRightLeg())
					getRightLeg()->setRenderImportance(1.0f);

				if (getLeftArm())
					getLeftArm()->setRenderImportance(1.0f);

				if (getRightArm())
					getRightArm()->setRenderImportance(1.0f);

				if (getTorso())
					getTorso()->setRenderImportance(1.0f);
			}
		}
	}

	void Humanoid::render2d(Adorn* adorn)
	{
		if (this != modelIsCharacter(Network::Players::findLocalCharacter(this)))
		{
			Workspace* workspace = Workspace::findWorkspace(this);
			if (workspace)
				renderMultiplayer(adorn, workspace->getGCamera());
		}
	}

	static void breakJoints(Instance* instance)
	{
		PartInstance* part = Instance::fastDynamicCast<PartInstance>(instance);
		if (!part)
			instance->for_eachChild(&breakJoints);
		else
			part->destroyJoints();
	}

	void Humanoid::onChangedHealth(const Reflection::PropertyDescriptor&)
	{
		health = G3D::clamp(health, 0.0f, maxHealth);

		if (health == 0.0f && !imDead)
		{
			imDead = true;

			FallingDown* fallingDown = new FallingDown(this, Math::inf());
			if (fallingDown != currentState.get())
				currentState.reset(fallingDown);

			if (getParent())
				getParent()->for_eachChild(breakJoints);

			event_Died.fire(this);
		}
	}

	void Humanoid::moveTo(const G3D::Vector3& worldPosition, PartInstance* part)
	{
		if (part)
		{
			G3D::Vector3 local = part->getCoordinateFrame().pointToObjectSpace(worldPosition);

			walkToPoint = local + G3D::Vector3::unitX();
			walkToPart.reset();

			setWalkToPart(part);
			setWalkToPoint(local);
		}
	}

	void Humanoid::moveTo2(G3D::Vector3 worldPosition, boost::shared_ptr<Instance> part)
	{
		PartInstance* p = fastDynamicCast<PartInstance>(part.get());
		if (!p)
			throw std::runtime_error("Argument error: A Part is required");

		moveTo(worldPosition, p);
	}

	void Humanoid::setWalkDirection(const G3D::Vector3& value)
	{
		if (walkDirection != value)
		{
			if (value == G3D::Vector3::zero())
			{
				walkDirection = value;
			}
			else
			{
				walkDirection = G3D::Vector3(value.x, 0.0f, value.z).direction();
				walkMode = DIRECTION_MOVE;
			}

			raisePropertyChanged(propWalkDirection);
		}
	}

	void Humanoid::setWalkRotationalVelocity(const float& value)
	{
		if (walkRotationalVelocity != value)
		{
			walkRotationalVelocity = value;
			raisePropertyChanged(propWalkRotationalVelocity);
		}
	}

	void Humanoid::setWalkToPoint(const G3D::Vector3& value)
	{
		if (walkToPoint != value)
		{
			walkToPoint = value;
			raisePropertyChanged(propWalkToPoint);
		}

		walkMode = (walkMode == HAS_PART) ? CLICK_TO_MOVE : HAS_POINT;
		walkTimer = (walkMode == CLICK_TO_MOVE) ? 240 : 0;
	}

	void Humanoid::setWalkToPart(PartInstance* value)
	{
		if (walkToPart.get() != value)
		{
			walkToPart = shared_from(value);
			raisePropertyChanged(propWalkToPart);
		}

		walkMode = (walkMode == HAS_POINT) ? CLICK_TO_MOVE : HAS_PART;
		walkTimer = (walkMode == CLICK_TO_MOVE) ? 240 : 0;
	}

	void Humanoid::setJump(bool value)
	{
		if (jump != value)
		{
			jump = value;
			raisePropertyChanged(propJump);
		}
	}

	void Humanoid::setSit(bool value)
	{
		if (sit != value)
		{
			sit = value;
			raisePropertyChanged(propSit);
		}
	}

	void Humanoid::setTargetPoint(const G3D::Vector3& value)
	{
		if (value != targetPoint)
		{
			targetPoint = value;
			raisePropertyChanged(propTargetPoint);
		}
	}

	void Humanoid::setLeftLeg(PartInstance* value)
	{
		if (leftLeg.get() != value)
		{
			leftLeg = shared_from(value);
			raisePropertyChanged(propLeftLeg);
		}
	}

	void Humanoid::setRightLeg(PartInstance* value)
	{
		if (rightLeg.get() != value)
		{
			rightLeg = shared_from(value);
			raisePropertyChanged(propRightLeg);
		}
	}


	void Humanoid::setTorso(PartInstance* value)
	{
		if (torso.get() != value)
		{
			torso = shared_from(value);
			raisePropertyChanged(propTorso);
		}
	}
}
