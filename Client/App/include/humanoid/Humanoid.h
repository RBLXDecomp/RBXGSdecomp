#pragma once
#include "util/RunStateOwner.h"
#include "util/IControllable.h"
#include "util/IRenderable.h"
#include "v8kernel/Connector.h"
#include "v8datamodel/ICharacterSubject.h"
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
	class Controller;

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

	public:
		class State : public INamed
		{
		protected:
			Humanoid* humanoid;

		protected:
			State(Humanoid* humanoid)
				: humanoid(humanoid)
			{
			}
		public:
			virtual ~State()
			{
			}
		public:
			virtual void onComputeForce(const float) = 0;
			virtual State* onStep(const float, Controller&) = 0;
			virtual bool canSleep() const = 0;

			virtual float getIntendedRotationAboutYAxis() const
			{
				return 0.0f;
			}
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
		mutable boost::shared_ptr<PartInstance> head;
		mutable boost::shared_ptr<PartInstance> torso;
		mutable boost::shared_ptr<PartInstance> leftLeg;
		mutable boost::shared_ptr<PartInstance> rightLeg;
		mutable boost::shared_ptr<PartInstance> rightArm;
		mutable boost::shared_ptr<PartInstance> leftArm;
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
		Humanoid();
		virtual ~Humanoid();
	public:
		void setHealth(float);
		float relativeHealth() const
		{
			return propHealth.getValue(this) / propMaxHealth.getValue(this);
		}

		Body* getTorsoBody();
		Body* getRootBody();
		void setWalkDirection(const G3D::Vector3& value);

		G3D::Vector3 getWalkDirection() const
		{
			return walkDirection;
		}

		void setWalkRotationalVelocity(const float& value);

		float getWalkRotationalVelocity() const
		{
			return walkRotationalVelocity;
		}

		void setWalkToPoint(const G3D::Vector3& value);

		const G3D::Vector3& getWalkToPoint() const
		{
			return walkToPoint;
		}

		void setWalkToPart(PartInstance* value);

		PartInstance* getWalkToPart() const
		{
			return walkToPart.get();
		}

		void setJump(bool value);

		bool getJump() const
		{
			return jump;
		}

		void setSit(bool value);

		bool getSit() const
		{
			return sit;
		}

		bool canSit() const
		{
			return !sit && currentState->getName() == "Running";
		}

		void setTargetPoint(const G3D::Vector3& value);

		const G3D::Vector3& getTargetPoint() const
		{
			return targetPoint;
		}

		void moveTo(const G3D::Vector3& worldPosition, PartInstance* part);
		void moveTo2(G3D::Vector3 worldPosition, boost::shared_ptr<Instance> part);
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
		void setTorso(PartInstance* value);
		void setLeftLeg(PartInstance* value);
		void setRightLeg(PartInstance* value);
		World* getWorld() const
		{
			return world;
		}
	private:
		void onChangedHealth(const Reflection::PropertyDescriptor&);
		void setState(State*);
		void checkForJointDeath();
		bool hasWalkToPoint(G3D::Vector3& worldPosition) const;
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onAncestorChanged(const AncestorChanged& event);
		virtual bool askSetParent(const Instance*) const;
		virtual bool shouldRender2d() const;
		virtual bool shouldRender3dAdorn() const;
		virtual void render2d(Adorn* adorn);
		virtual void render3dAdorn(Adorn* adorn);
		void renderMultiplayer(Adorn*, const G3D::GCamera&);
		virtual bool isControllable() const;
		virtual void onEvent(const RunService* source, Stepped event);
		virtual void computeForce(const float dt, bool throttling);
		virtual const G3D::CoordinateFrame getLocation() const;
		virtual ContactManager* getContactManager();
		virtual void tellCameraNear(float distance);
		G3D::Vector3 getIntendedMovementVector(bool);
		virtual G3D::Vector3 getIntendedMovementVector();
		virtual float getIntendedRotationAboutYAxis();

		virtual void getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives)
		{
			getIgnorePrims(primitives);
		}

		virtual void cameraSetWalkOrientation(float, bool);
		void onLocalHumanoidEnteringWorkspace();
	public:
		virtual void getIgnorePrims(std::vector<const Primitive*>&);
	  
	public:
		static Humanoid* modelIsCharacter(const Instance* testModel);
		static Humanoid* getLocalHumanoidFromContext(const Instance* context);
		static PartInstance* getLocalHeadFromContext(const Instance* context);
		static ModelInstance* getCharacterFromHumanoid(Humanoid*);
		static PartInstance* getHeadFromCharacter(const ModelInstance* character);
		static Weld* getGrip(const Instance*);
		static const float walkSpeed();
		static void renderWaypoint(Adorn* adorn, const G3D::Vector3& waypoint);
	};
}
