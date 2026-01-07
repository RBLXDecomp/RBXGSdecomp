#pragma once
#include <boost/shared_ptr.hpp>
#include "v8world/Joint.h"
#include "v8tree/Instance.h"
#include "v8datamodel/PartInstance.h"
#include "util/IRenderable.h"
#include "reflection/reflection.h"

namespace RBX 
{
	extern const char* sJointInstance;
	class JointInstance : public DescribedNonCreatable<JointInstance, Instance, &sJointInstance>,
						  public IRenderable,
						  public IJointOwner
	{
	protected:
		Joint* joint;

		virtual bool shouldRender3dAdorn() const;
		virtual void render3dAdorn(Adorn* adorn);

	public:
		JointInstance(Joint* joint);
		~JointInstance();
    };

	extern const char* sAutoJoint;
	class AutoJoint : public DescribedNonCreatable<AutoJoint, JointInstance, &sAutoJoint>
	{
	private:
		boost::shared_ptr<PartInstance> part[2];

		void setPart(int i, PartInstance* value);
		void setPartNull(int);
		void setPrimitiveFromPart(int);

		virtual bool askSetParent(const Instance* instance) const;
		virtual void onAncestorChanged(const AncestorChanged& event);

		World* computeWorld();

	public:
		AutoJoint(Joint* joint);
		virtual ~AutoJoint();

		PartInstance* getPart0() const;
		PartInstance* getPart1() const;

		void setPart0(PartInstance* value);
		void setPart1(PartInstance* value);

		const G3D::CoordinateFrame& getC0() const;
		const G3D::CoordinateFrame& getC1() const;

		void setC0(const G3D::CoordinateFrame& value);
		void setC1(const G3D::CoordinateFrame& value);
	};


	extern const char* sSnap;
	class Snap : public DescribedCreatable<Snap, AutoJoint, &sSnap>
	{
	public:
		Snap(Joint* joint);
		Snap();
		virtual ~Snap();
	};

	extern const char* sWeld;
	class Weld : public DescribedCreatable<Weld, AutoJoint, &sWeld>
	{
	public:
		Weld(Joint* joint);
		Weld();
		virtual ~Weld();
	};

	extern const char* sGlue;
	class Glue : public DescribedCreatable<Glue, AutoJoint, &sGlue>
	{
	public:
		Glue(Joint* joint);
		Glue();
		virtual ~Glue();
	};

	extern const char* sRotate;
	class Rotate : public DescribedCreatable<Rotate, AutoJoint, &sRotate>
	{
	public:
		Rotate(Joint* joint);
		Rotate();
		virtual ~Rotate();
	};

	extern const char* sRotateP;
	class RotateP : public DescribedCreatable<RotateP, AutoJoint, &sRotateP>
	{
	public:
		RotateP(Joint* joint);
		RotateP();
		virtual ~RotateP();
	};

	extern const char* sRotateV;
	class RotateV : public DescribedCreatable<RotateV, AutoJoint, &sRotateV>
	{
	public:
		RotateV(Joint* joint);
		RotateV();
		virtual ~RotateV();
	};

	extern const char* sMotor;
	class Motor : public DescribedCreatable<Motor, AutoJoint, &sMotor>
	{
	private:
		const MotorJoint* getMotorJoint() const;
		MotorJoint* getMotorJoint();
		void renderJointCylinder(Adorn*, int);

	public:
		Motor(Joint* joint);
		Motor();

		float getMaxVelocity() const;
		void setMaxVelocity(float value);

		float getDesiredAngle() const;
		void setDesiredAngle(float value);

		float getCurrentAngle() const;
		void setCurrentAngle(float value);

		virtual ~Motor();
	};
}
