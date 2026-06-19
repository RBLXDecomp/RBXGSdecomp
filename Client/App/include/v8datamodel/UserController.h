#pragma once
#include "v8tree/Service.h"
#include "v8world/Controller.h"
#include "util/UserInputBase.h"
#include "util/RunStateOwner.h"

namespace RBX
{
	class PVInstance;

	extern const char* sControllerService;
	class ControllerService : public DescribedCreatable<ControllerService, Instance, &sControllerService>, public Service
	{
	private:
		UserInputBase* hardwareDevice;
		G3D::ReferenceCountedPointer<Controller> playerController;
		G3D::ReferenceCountedPointer<Controller> keyboardPrimaryController;
		G3D::ReferenceCountedPointer<Controller> keyboardSecondaryController;
		G3D::ReferenceCountedPointer<Controller> nullController;

	public:
		ControllerService();
		G3D::ReferenceCountedPointer<Controller> getController(Controller::ControllerType type, const PVInstance* controlledInstance);

		const UserInputBase* getHardwareDevice() const
		{
			return hardwareDevice;
		}

		UserInputBase* getHardwareDevice()
		{
			return hardwareDevice;
		}

		void setHardwareDevice(UserInputBase*);
	};

	class HardwareController : public Controller
	{
	private:
		ControllerService* controllerService;

	protected:
		HardwareController(ControllerService* service)
			: controllerService(service)
		{
		}

		const UserInputBase* getHardwareDevice() const
		{
			return controllerService ? controllerService->getHardwareDevice() : NULL;
		}

		virtual bool hasIntelligence() const
		{
			return true;
		}

		virtual bool isUserController() const
		{
			return true;
		}
	};

	class KeyboardPrimaryController : public HardwareController
	{
	public:
		KeyboardPrimaryController(ControllerService* service)
			: HardwareController(service)
		{
		}

		virtual float getValue(Controller::InputType) const;

		virtual Controller::ControllerType getControllerType() const
		{
			return Controller::PRIMARY_CONTROLLER;
		}
	};

	class KeyboardSecondaryController : public HardwareController
	{
	public:
		KeyboardSecondaryController(ControllerService* service)
			: HardwareController(service)
		{
		}

		virtual float getValue(Controller::InputType) const;

		virtual Controller::ControllerType getControllerType() const
		{
			return Controller::SECONDARY_CONTROLLER;
		}
	};

	class SteppingController : public Controller, public Listener<RunService, Heartbeat>
	{
	private:
		boost::shared_ptr<RunService> runService;
	protected:
		boost::weak_ptr<const PVInstance> controlledInstance;

	protected:
		SteppingController(const PVInstance* _controlledInstance);
		virtual ~SteppingController();
	};

	class PlayerController : public SteppingController
	{
	private:
		boost::weak_ptr<ControllerService> controllerService;
		int stepsRotating;

	private:
		const UserInputBase* getHardwareDevice() const;

		virtual bool hasIntelligence() const
		{
			return true;
		}

		virtual bool isUserController() const
		{
			return true;
		}

		virtual Controller::ControllerType getControllerType() const
		{
			return PLAYER_CONTROLLER;
		}

		virtual void onEvent(const RunService*, Heartbeat);
	public:
		PlayerController(ControllerService* _controllerService, const PVInstance* _controlledInstance);
	};

	class AIController : public SteppingController
	{
	private:
		float retargetTime;
	protected:
		float buffer[Controller::NUM_INPUT_TYPES];
		boost::weak_ptr<const PVInstance> target;

	protected:
		AIController(const PVInstance* controlledInstance);
		virtual ~AIController();
		void updateTarget(Controller::ControllerType, float);
		G3D::Vector3 getLocalTargetPosition() const;
		void chaseTarget(const G3D::Vector3&);

		virtual bool hasIntelligence() const
		{
			return true;
		}

		virtual bool isUserController() const
		{
			return false;
		}

		virtual float getValue(Controller::InputType inputType) const;
		virtual void updateBuffer(float) = 0;

		virtual void onEvent(const RunService* source, Heartbeat event)
		{
			updateBuffer(event.time);
		}
	};

	class AIChaseController : public AIController
	{
	protected:
		virtual void updateBuffer(float time);
	public:
		AIChaseController(const PVInstance* controlledInstance)
			: AIController(controlledInstance)
		{
		}

		virtual Controller::ControllerType getControllerType() const
		{
			return AI_CHASE_CONTROLLER;
		}
	};

	class AIFleeController : public AIController
	{
	protected:
		virtual void updateBuffer(float time);
	public:
		AIFleeController(const PVInstance* controlledInstance)
			: AIController(controlledInstance)
		{
		}

		virtual Controller::ControllerType getControllerType() const
		{
			return AI_FLEE_CONTROLLER;
		}
	};
}
