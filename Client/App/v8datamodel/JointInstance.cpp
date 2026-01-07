#include "v8datamodel/JointInstance.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/Feature.h"
#include "v8world/Joint.h"
#include "v8world/SnapJoint.h"
#include "v8world/WeldJoint.h"
#include "v8world/GlueJoint.h"
#include "v8world/RotateJoint.h"
#include "v8world/MotorJoint.h"
#include "util/Debug.h"

static const RBX::Reflection::PropDescriptor<RBX::AutoJoint, RBX::PartInstance*> prop_Part0("Part0", "Data", &RBX::AutoJoint::getPart0, &RBX::AutoJoint::setPart0, RBX::Reflection::PropertyDescriptor::LEGACY); 
static const RBX::Reflection::PropDescriptor<RBX::AutoJoint, RBX::PartInstance*> prop_Part1("Part1", "Data", &RBX::AutoJoint::getPart1, &RBX::AutoJoint::setPart1, RBX::Reflection::PropertyDescriptor::LEGACY); 

static const RBX::Reflection::PropDescriptor<RBX::AutoJoint, G3D::CoordinateFrame> prop_C1("C1", "Data", &RBX::AutoJoint::getC1, &RBX::AutoJoint::setC1, RBX::Reflection::PropertyDescriptor::LEGACY);
static const RBX::Reflection::PropDescriptor<RBX::AutoJoint, G3D::CoordinateFrame> prop_C0("C0", "Data", &RBX::AutoJoint::getC0, &RBX::AutoJoint::setC0, RBX::Reflection::PropertyDescriptor::LEGACY);

// TODO: checkk that Motor::getMaxVelocity gets linked as VelocityMotor::getMaxVelocity
static const RBX::Reflection::PropDescriptor<RBX::Motor, float> prop_MaxVelocity("MaxVelocity", "Data", &RBX::Motor::getMaxVelocity, &RBX::Motor::setMaxVelocity, RBX::Reflection::PropertyDescriptor::LEGACY);
static const RBX::Reflection::PropDescriptor<RBX::Motor, float> prop_DesiredAngle("DesiredAngle", "Data", &RBX::Motor::getDesiredAngle, &RBX::Motor::setDesiredAngle, RBX::Reflection::PropertyDescriptor::LEGACY);
static const RBX::Reflection::PropDescriptor<RBX::Motor, float> prop_CurrentAngle("CurrentAngle", "Data", &RBX::Motor::getCurrentAngle, &RBX::Motor::setCurrentAngle, RBX::Reflection::PropertyDescriptor::LEGACY);

namespace RBX
{
	const char* sJointInstance = "JointInstance";

	JointInstance::JointInstance(Joint* joint)
		: joint(joint)
	{
		joint->setJointOwner(this);
	}

	// NEEDS: ~Instance
	JointInstance::~JointInstance()
	{
		RBXASSERT(getParent() == NULL);
		RBXASSERT(joint->getWorld() == NULL);

		joint->setJointOwner(NULL);
		delete joint;
		joint = NULL;
	}

	bool JointInstance::shouldRender3dAdorn() const
	{
		return PartInstance::showSpanningTree;
	}

	void JointInstance::render3dAdorn(Adorn* adorn)
	{
		RBXASSERT(shouldRender3dAdorn());

		if (Joint::isAssemblyJoint(joint))
		{
			G3D::Color3 color3;

			Primitive* prim0 = joint->getPrimitive(0);
			Primitive* prim1 = joint->getPrimitive(1);

			G3D::Vector3 vec0 = prim0
				? prim0->getCoordinateFrame().translation
				: G3D::Vector3::zero();

			G3D::Vector3 vec1 = prim1
				? prim1->getCoordinateFrame().translation
				: G3D::Vector3::zero();

			if (joint->getActive())
				color3 = G3D::Color3::green();
			else
				color3 = G3D::Color3::red();

			adorn->setObjectToWorldMatrix(G3D::CoordinateFrame());

			G3D::Color4 color4(color3);
			adorn->lineSegment(
				G3D::LineSegment::fromTwoPoints(vec0, vec1),
				color4,
				20.0);
		}
	}

	const char* sAutoJoint = "AutoJoint";

	AutoJoint::AutoJoint(Joint* _joint)
		: DescribedNonCreatable(_joint)
	{
		PartInstance* part0 = PartInstance::fromPrimitive(joint->getPrimitive(0));
		PartInstance* part1 = PartInstance::fromPrimitive(joint->getPrimitive(1));

		part[0] = shared_from(part0);
		part[1] = shared_from(part1);
	}

	AutoJoint::~AutoJoint()
	{
		setPart(0, NULL);
		setPart(1, NULL);
	}

	PartInstance* AutoJoint::getPart0() const
	{
		return part[0].get();
	}

	PartInstance* AutoJoint::getPart1() const
	{
		return part[1].get();
	}

	void AutoJoint::setPart0(PartInstance* value)
	{
		setPart(0, value);
		raisePropertyChanged(prop_Part0);
	}

	void AutoJoint::setPart1(PartInstance* value)
	{
		setPart(1, value);
		raisePropertyChanged(prop_Part1);
	}

	bool AutoJoint::askSetParent(const Instance* instance) const
	{
		return instance->getDescriptor() == Instance::classDescriptor()
				|| instance->getDescriptor() == PartInstance::classDescriptor();
	}

	void AutoJoint::onAncestorChanged(const AncestorChanged& event)
	{
		Instance::onAncestorChanged(event);

		World* oldWorld = joint->getWorld();
		World* wsWorld;

		Instance* parent = getParent();

		if (parent && parent->getDescriptor() == Instance::classDescriptor())
		{
			Workspace* workspace = ServiceProvider::find<Workspace>(this);

			if (workspace)
				wsWorld = workspace->getWorld();
			else
				wsWorld = NULL;
		}
		else
		{
			wsWorld = Workspace::getWorldIfInWorkspace(this);
		}

		if (oldWorld != wsWorld)
		{
			if (oldWorld)
			{
				oldWorld->removeJoint(joint);
				RBXASSERT(!joint->getWorld());
			}

			if (wsWorld)
			{
				wsWorld->insertJoint(joint);
				RBXASSERT(joint->getWorld());
			}
		}
	}

	void AutoJoint::setPart(int i, PartInstance* value)
	{
		if (part[i].get() != value)
		{
			RBX::Primitive* ptr;

			part[i] = RBX::shared_from(value);

			if (value)
				ptr = value->getPrimitive();
			else
				ptr = NULL;

			joint->setPrimitive(i, ptr);
		}
	}

	const G3D::CoordinateFrame& AutoJoint::getC0() const
	{
		return joint->getJointCoord(0);
	}

	void AutoJoint::setC0(const G3D::CoordinateFrame& value)
	{
		joint->setJointCoord(0, value);
		raisePropertyChanged(prop_C0);
	}

	const G3D::CoordinateFrame& AutoJoint::getC1() const
	{
		return joint->getJointCoord(1);
	}

	void AutoJoint::setC1(const G3D::CoordinateFrame& value)
	{
		joint->setJointCoord(1, value);
		raisePropertyChanged(prop_C1);
	}

	const char* sSnap = "Snap";

	Snap::Snap(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::SNAP_JOINT);
		setName("Snap");
	}

	Snap::Snap()
		: DescribedCreatable(new SnapJoint())
	{
		setName("Snap");
	}

	const char* sWeld = "Weld";

	Weld::Weld(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::WELD_JOINT);
	}

	Weld::Weld()
		: DescribedCreatable(new WeldJoint())
	{
	}

	const char* sGlue = "Glue";

	Glue::Glue(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::GLUE_JOINT);
	}

	Glue::Glue()
		: DescribedCreatable(new GlueJoint())
	{
	}

	const char* sRotate = "Rotate";

	Rotate::Rotate(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::ROTATE_JOINT);
	}

	Rotate::Rotate()
		: DescribedCreatable(new RotateJoint())
	{
	}

	const char* sRotateP = "RotateP";

	RotateP::RotateP(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::ROTATE_P_JOINT);
	}

	RotateP::RotateP()
		: DescribedCreatable(new RotatePJoint())
	{
	}

	const char* sRotateV = "RotateV";

	RotateV::RotateV(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::ROTATE_V_JOINT);
	}

	RotateV::RotateV()
		: DescribedCreatable(new RotateVJoint())
	{
	}

	const char* sMotor = "Motor";

	Motor::Motor(Joint* joint)
		: DescribedCreatable(joint)
	{
		RBXASSERT(joint->getJointType() == Joint::MOTOR_JOINT);
	}

	Motor::Motor()
		: DescribedCreatable(new MotorJoint())
	{
		setName("Motor");
	}

	float Motor::getMaxVelocity() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->maxVelocity;
	}

	void Motor::setMaxVelocity(float value)
	{
		if (value != getMaxVelocity())
		{
			rbx_static_cast<MotorJoint*>(joint)->maxVelocity = value;
			raisePropertyChanged(prop_MaxVelocity);
		}
	}

	float Motor::getDesiredAngle() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->desiredAngle;
	}

	void Motor::setDesiredAngle(float value)
	{
		if (value != getDesiredAngle())
		{
			rbx_static_cast<MotorJoint*>(joint)->desiredAngle = value;
			raisePropertyChanged(prop_DesiredAngle);
		}
	}

	float Motor::getCurrentAngle() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->getCurrentAngle();
	}

	void Motor::setCurrentAngle(float value)
	{
		if (value != getCurrentAngle())
		{
			rbx_static_cast<MotorJoint*>(joint)->setCurrentAngle(value);
			raisePropertyChanged(prop_CurrentAngle);
		}
	}
}
