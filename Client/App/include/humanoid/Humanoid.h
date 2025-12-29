#pragma once
#include "v8tree/Instance.h"
#include "v8kernel/Connector.h"
#include "util/RunStateOwner.h"
#include "v8datamodel/ICharacterSubject.h"
#include "util/IControllable.h"
#include "util/IRenderable.h"
#include <G3D/Vector3.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/GCamera.h>

namespace RBX
{
	class Weld;
	class Primitive;
	class World;
	class PartInstance;
	class ModelInstance;
	class JointInstance;
	class Accoutrement;

	extern const char* sHumanoid;
	class Humanoid : public DescribedCreatable<Humanoid, Instance, &sHumanoid>,
					 public Connector,
					 public IControllable,
					 public IRenderable,
					 public ICharacterSubject,
					 public Listener<RunService, Stepped>
	{
	private:
		enum WalkMode
		{
			HAS_PART,
			HAS_POINT,
			CLICK_TO_MOVE,
			DIRECTION_MOVE
		};

		class State : public INamed
		{
		protected:
			Humanoid* humanoid;

		public:
			//State(const State&);
		protected:
			State(Humanoid*);
		public:
			virtual ~State();
		public:
			virtual void onComputeForce(const float);
			virtual State* onStep(const float, Controller&);
			virtual bool canSleep() const;
			virtual float getIntendedRotationAboutYAxis() const;
		};

	private:
		float health;
		float maxHealth;
		float walkRotationalVelocity;
		int walkTimer;
		WalkMode walkMode;
		boost::shared_ptr<PartInstance> walkToPart;
		G3D::Vector3 walkToPoint;
		G3D::Vector3 walkDirection;
		G3D::Vector3 targetPoint;
		bool jump : 1;
		bool imDead : 1;
		bool hadHeadJoint : 1;
		bool sit : 1;
		boost::shared_ptr<PartInstance> head;
		boost::shared_ptr<PartInstance> torso;
		boost::shared_ptr<PartInstance> leftLeg;
		boost::shared_ptr<PartInstance> rightLeg;
		boost::shared_ptr<PartInstance> rightArm;
		boost::shared_ptr<PartInstance> leftArm;
		World* world;
		std::auto_ptr<State> currentState;

	public:
		static Reflection::BoundProp<float, 1> propHealth;
		static Reflection::BoundProp<float, 1> propMaxHealth;
		static Reflection::SignalDesc<Humanoid, void(void)> event_Died;
		static Reflection::SignalDesc<Humanoid, void(float)> event_Running;
		static Reflection::SignalDesc<Humanoid, void(float)> event_Climbing;
		static Reflection::SignalDesc<Humanoid, void(bool)> event_Jumping;
		static Reflection::SignalDesc<Humanoid, void(bool)> event_FreeFalling;
		static Reflection::SignalDesc<Humanoid, void(bool)> event_GettingUp;
		static Reflection::SignalDesc<Humanoid, void(bool)> event_FallingDown;
		static Reflection::SignalDesc<Humanoid, void(bool)> event_Seated;
	  
	private:
		void resetTimer();
		void getPrimitives(std::vector<Primitive*>&);
		void getAccoutrements(std::vector<boost::shared_ptr<Accoutrement>>&);
	public:
		//Humanoid(Humanoid&);
		Humanoid();
		virtual ~Humanoid();
	public:
		void setHealth(float);
		float relativeHealth() const;
		Body* getTorsoBody();
		Body* getRootBody();
		void setWalkDirection(const G3D::Vector3&);
		G3D::Vector3 getWalkDirection() const;
		void setWalkRotationalVelocity(const float&);
		float getWalkRotationalVelocity() const;
		void setWalkToPoint(const G3D::Vector3&);
		const G3D::Vector3& getWalkToPoint() const;
		void setWalkToPart(PartInstance*);
		PartInstance* getWalkToPart() const;
		void setJump(bool);
		bool getJump() const;
		void setSit(bool);
		bool getSit() const;
		bool canSit() const;
		void setTargetPoint(const G3D::Vector3&);
		const G3D::Vector3& getTargetPoint() const;
		void moveTo(const G3D::Vector3&, PartInstance*);
		void moveTo2(G3D::Vector3, boost::shared_ptr<Instance>);
		G3D::Vector3 updateWalkDirection();
		void buildJoints();
		Primitive* getHeadPrimitive();
		Primitive* getTorsoPrimitive() const;
		Primitive* getLeftLegPrimitive();
		Primitive* getRightLegPrimitive();
		Primitive* getLeftArmPrimitive();
		Primitive* getRightArmPrimitive();
		PartInstance* getHead() const;
		PartInstance* getTorso() const;
		PartInstance* getLeftLeg() const;
		PartInstance* getRightLeg() const;
		PartInstance* getLeftArm() const;
		PartInstance* getRightArm() const;
		G3D::CoordinateFrame getTopOfHead() const;
		G3D::CoordinateFrame getRightArmGrip() const;
		PartInstance* findRightArm() const;
		JointInstance* findRightShoulder() const;
		float getTorsoHeading() const;
		void setTorso(PartInstance*);
		void setLeftLeg(PartInstance*);
		void setRightLeg(PartInstance*);
		World* getWorld() const;
	private:
		void onChangedHealth(const Reflection::PropertyDescriptor&);
		void setState(State*);
		void checkForJointDeath();
		bool hasWalkToPoint(G3D::Vector3&) const;
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onAncestorChanged(const AncestorChanged&);
		virtual bool askSetParent(const Instance*) const;
		virtual bool shouldRender2d() const;
		virtual bool shouldRender3dAdorn() const;
		virtual void render2d(Adorn*);
		virtual void render3dAdorn(Adorn*);
		void renderMultiplayer(Adorn*, const G3D::GCamera&);
		virtual bool isControllable() const;
		virtual void onEvent(const RunService*, Stepped);
		virtual void computeForce(const float, bool);
		virtual const G3D::CoordinateFrame getLocation() const;
		virtual ContactManager* getContactManager();
		virtual void tellCameraNear(float);
		G3D::Vector3 getIntendedMovementVector(bool);
		virtual G3D::Vector3 getIntendedMovementVector();
		virtual float getIntendedRotationAboutYAxis();
		virtual void getCameraIgnorePrimitives(std::vector<const Primitive*>&);
		virtual void cameraSetWalkOrientation(float, bool);
		void onLocalHumanoidEnteringWorkspace();
	public:
		virtual void getIgnorePrims(std::vector<const Primitive*>&);
	public:
		//Humanoid& operator=(Humanoid&);
	  
	public:
		static Humanoid* modelIsCharacter(const Instance*);
		static Humanoid* getLocalHumanoidFromContext(const Instance*);
		static PartInstance* getLocalHeadFromContext(const Instance*);
		static ModelInstance* getCharacterFromHumanoid(Humanoid*);
		static PartInstance* getHeadFromCharacter(const ModelInstance*);
		static Weld* getGrip(const Instance*);
		static const float walkSpeed();
		static void renderWaypoint(Adorn*, const G3D::Vector3&);
	};
}
