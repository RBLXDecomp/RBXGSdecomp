#include "v8datamodel/Feature.h"
#include "v8datamodel/Workspace.h"
#include "v8world/MotorJoint.h"
#include "AppDraw/Draw.h"
#include "AppDraw/DrawAdorn.h"

namespace RBX 
{
	static Reflection::RefPropDescriptor<VelocityMotor, Hole> prop_Hole         ("Hole",         "Data", &VelocityMotor::getHole,         &VelocityMotor::setHole,         Reflection::PropertyDescriptor::LEGACY);
	static Reflection::PropDescriptor<VelocityMotor, float>   prop_MaxVelocity  ("MaxVelocity",  "Data", &VelocityMotor::getMaxVelocity,  &VelocityMotor::setMaxVelocity,  Reflection::PropertyDescriptor::LEGACY);
	static Reflection::PropDescriptor<VelocityMotor, float>   prop_DesiredAngle ("DesiredAngle", "Data", &VelocityMotor::getDesiredAngle, &VelocityMotor::setDesiredAngle, Reflection::PropertyDescriptor::LEGACY);
	static Reflection::PropDescriptor<VelocityMotor, float>   prop_CurrentAngle ("CurrentAngle", "Data", &VelocityMotor::getCurrentAngle, &VelocityMotor::setCurrentAngle, Reflection::PropertyDescriptor::LEGACY);

	Reflection::EnumDesc<Feature::TopBottom>::EnumDesc()
		: EnumDescriptor("TopBottom", typeid(Feature::TopBottom))
	{
		addPair(Feature::TOP,       "Top");
		addPair(Feature::CENTER_TB, "Center");
		addPair(Feature::BOTTOM,    "Bottom");
	}

	Reflection::EnumDesc<Feature::LeftRight>::EnumDesc()
		: EnumDescriptor("LeftRight", typeid(Feature::LeftRight))
	{
		addPair(Feature::LEFT,      "Left");
		addPair(Feature::CENTER_LR, "Center");
		addPair(Feature::RIGHT,     "Right");
	}

	Reflection::EnumDesc<Feature::InOut>::EnumDesc()
		: EnumDescriptor("InOut", typeid(Feature::InOut))
	{
		addPair(Feature::EDGE,      "Edge");
		addPair(Feature::INSET,     "Inset");
		addPair(Feature::CENTER_IO, "Center");
	}

	static Reflection::EnumPropDescriptor<Feature, NormalId>           prop_FaceId    ("FaceId",    "Data", &Feature::getFaceId,    &Feature::setFaceId,    Reflection::PropertyDescriptor::LEGACY);
	static Reflection::EnumPropDescriptor<Feature, Feature::TopBottom> prop_TopBottom ("TopBottom", "Data", &Feature::getTopBottom, &Feature::setTopBottom, Reflection::PropertyDescriptor::LEGACY);
	static Reflection::EnumPropDescriptor<Feature, Feature::LeftRight> prop_LeftRight ("LeftRight", "Data", &Feature::getLeftRight, &Feature::setLeftRight, Reflection::PropertyDescriptor::LEGACY);
	static Reflection::EnumPropDescriptor<Feature, Feature::InOut>     prop_InOut     ("InOut",     "Data", &Feature::getInOut,     &Feature::setInOut,     Reflection::PropertyDescriptor::LEGACY);

    const char* sFeature = "Feature";

	Feature::Feature()
		: faceId(NORM_X),
		  topBottom(CENTER_TB),
		  leftRight(CENTER_LR),
		  inOut(CENTER_IO)
	{
		setName("Feature");
	}

	Feature::~Feature()
	{
	}

	// TODO: 98.92% match
	bool Feature::getRenderCoord(G3D::CoordinateFrame& c) const
	{
		PartInstance* part = fastDynamicCast<PartInstance>(getParent());

		if (part)
		{
			// for some reason local is being taken as operator*'s LHS
			// and not part->getCoordinateFrame()
			G3D::CoordinateFrame local = computeLocalCoordinateFrame();
			c = part->getCoordinateFrame()*local;

			return true;
		}
		else
		{
			return false;
		}
	}

	void Feature::render3dSelect(Adorn* adorn, SelectState selectState)
	{
		G3D::CoordinateFrame worldCoord;

		if (getRenderCoord(worldCoord))
			DrawAdorn::cylinder(adorn, worldCoord, 2, 0.3, 0.6, Draw::selectColor());
	}

	// TODO: 98.37% match
	G3D::CoordinateFrame Feature::computeLocalCoordinateFrame() const
	{
		PartInstance* part = fastDynamicCast<PartInstance>(getParent());

		if (!part)
			return G3D::CoordinateFrame();

		Extents extents = part->getPrimitive()->getExtentsLocal();

		G3D::Vector3 corner = extents.max();
		G3D::Vector3 uv = Math::vector3Abs(RBX::objectToUvw(corner, faceId));

		uv.z = 0.0f;

		switch (leftRight)
		{
			case LEFT:
				uv.x = -uv.x;
				break;

			case CENTER_LR:
				uv.x = 0.0f;
				break;
		}

		switch (topBottom)
		{
			case CENTER_TB:
				uv.y = 0.0f;
				break;

			case BOTTOM:
				uv.y = -uv.y;
				break;
		}

		switch (inOut)
		{
		case INSET:
			uv.x = RBX::Math::sign(uv.x) * std::max(0.0f, std::abs(uv.x) - 1.0f);
			uv.y = RBX::Math::sign(uv.y) * std::max(0.0f, std::abs(uv.y) - 1.0f);
			break;

		case CENTER_IO:
			uv.x = 0.0f;
			uv.y = 0.0f;
			break;
		}

		G3D::Vector3 uvInObject = RBX::uvwToObject(uv, faceId);
		G3D::Vector3 ptInObject = extents.faceCenter(faceId) + uvInObject;

		RBX::NormalId face = getCoordOrientation() == Z_OUT
							     ? faceId
							     : RBX::intToNormalId((faceId + NORM_X_NEG) % NORM_UNDEFINED);

		return G3D::CoordinateFrame(RBX::normalIdToMatrix3(face), ptInObject);
	}

	void Feature::setFaceId(NormalId value)
	{
		if (faceId != value)
		{
			faceId = value;
			raisePropertyChanged(prop_FaceId);
		}
	}

	void Feature::setTopBottom(TopBottom value)
	{
		if (topBottom != value)
		{
			topBottom = value;
			raisePropertyChanged(prop_TopBottom);
		}
	}

	void Feature::setLeftRight(LeftRight value)
	{
		if (leftRight != value)
		{
			leftRight = value;
			raisePropertyChanged(prop_LeftRight);
		}
	}

	void Feature::setInOut(InOut value)
	{
		if (inOut != value)
		{
			inOut = value;
			raisePropertyChanged(prop_InOut);
		}
	}

    const char* sHole = "Hole";

	Hole::Hole()
	{
		setName("Hole");
	}

	void Hole::render3dAdorn(Adorn* adorn)
	{
		G3D::CoordinateFrame worldCoord;

		if (getRenderCoord(worldCoord))
			DrawAdorn::cylinder(adorn, worldCoord, 2, 0.2, 0.3, G3D::Color3::black());
	}

    const char* sMotorFeature = "MotorFeature";

	MotorFeature::MotorFeature()
	{
		setName("MotorFeature");
	}

	void MotorFeature::render3dAdorn(Adorn* adorn)
	{
		G3D::CoordinateFrame worldCoord;

		if (getRenderCoord(worldCoord))
			DrawAdorn::cylinder(adorn, worldCoord, 2, 1.0, 0.3, G3D::Color3::yellow());
	}

    const char* sVelocityMotor = "VelocityMotor";

	VelocityMotor::VelocityMotor()
		: DescribedCreatable(new MotorJoint())
	{
		setName("VelocityMotor");
	}

	VelocityMotor::~VelocityMotor()
	{
	}

	// TODO: figure out where this can go, maybe it's not an inline? it's the
	// only way i can make this work sensibly though
	inline Primitive* _featureInline(Instance* instance)
	{
		if (instance)
		{
			PartInstance* part = Instance::fastDynamicCast<PartInstance>(instance->getParent());

			if (part)
				return part->getPrimitive();
		}

		return NULL;
	}

	void VelocityMotor::setPart(int i, Feature* feature)
	{
		Primitive* prim = _featureInline(feature);

		G3D::CoordinateFrame c = feature ? feature->computeLocalCoordinateFrame() : G3D::CoordinateFrame();

		if (motorJoint()->getPrimitive(i) != prim)
		{
			motorJoint()->setPrimitive(i, prim);

			if (prim)
				motorJoint()->setJointCoord(i, c);
		}
	}

	void VelocityMotor::onAncestorChanged(const AncestorChanged& event)
	{
		Instance::onAncestorChanged(event);

		setPart(0, fastDynamicCast<Feature>(this->getParent()));

		World* oldWorld = motorJoint()->getWorld();
		World* wsWorld = Workspace::getWorldIfInWorkspace(this);

		if (oldWorld != wsWorld)
		{
			if (oldWorld)
			{
				oldWorld->removeJoint(motorJoint());
				setPart(0, NULL);
				setPart(1, NULL);
				RBXASSERT(!motorJoint()->getWorld());
			}

			if (wsWorld)
			{
				wsWorld->insertJoint(motorJoint());
				RBXASSERT(motorJoint()->getWorld());
			}
		}
	}

	void VelocityMotor::onEvent_HoleAncestorChanged()
	{
		RBXASSERT(hole.get());
		setPart(1, hole.get());
	}

	void VelocityMotor::setHole(Hole* value)
	{
		if (value != getHole())
		{
			hole = RBX::shared_from(value);
			raisePropertyChanged(prop_Hole);
			setPart(1, getHole());

			if (getHole())
				holeAncestorChanged = event_ancestryChanged.connect(hole.get(), boost::bind(&VelocityMotor::onEvent_HoleAncestorChanged, this));
			else
				holeAncestorChanged.disconnect();
		}
	}

	float VelocityMotor::getMaxVelocity() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->maxVelocity;
	}

	void VelocityMotor::setMaxVelocity(float value)
	{
		if (value != getMaxVelocity())
		{
			rbx_static_cast<MotorJoint*>(joint)->maxVelocity = value;
			raisePropertyChanged(prop_MaxVelocity);
		}
	}

	float VelocityMotor::getDesiredAngle() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->desiredAngle;
	}

	void VelocityMotor::setDesiredAngle(float value)
	{
		if (value != getDesiredAngle())
		{
			rbx_static_cast<MotorJoint*>(joint)->desiredAngle = value;
			raisePropertyChanged(prop_DesiredAngle);
		}
	}

	float VelocityMotor::getCurrentAngle() const
	{
		return rbx_static_cast<MotorJoint*>(joint)->getCurrentAngle();
	}

	void VelocityMotor::setCurrentAngle(float value)
	{
		if (value != getCurrentAngle())
		{
			rbx_static_cast<MotorJoint*>(joint)->setCurrentAngle(value);
			raisePropertyChanged(prop_CurrentAngle);
		}
	}
}
