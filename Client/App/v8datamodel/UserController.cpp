#include "v8datamodel/PVInstance.h"
#include "v8datamodel/UserController.h"
#include <G3D/Vector3.h>

namespace RBX
{

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list

	ControllerService::ControllerService()
		: playerController(NULL),
		  hardwareDevice(NULL),
		  keyboardPrimaryController(new KeyboardPrimaryController(this)),
		  keyboardSecondaryController(new KeyboardSecondaryController(this)),
		  nullController(new NullController)
	{
	}

#pragma warning (pop)

	G3D::ReferenceCountedPointer<Controller> ControllerService::getController(Controller::ControllerType type, const PVInstance* controlledInstance)
	{
		switch (type)
		{
		case Controller::PLAYER_CONTROLLER:
			return new PlayerController(this, controlledInstance);

		case Controller::PRIMARY_CONTROLLER:
			return keyboardPrimaryController;

		case Controller::SECONDARY_CONTROLLER:
			return keyboardSecondaryController;

		case Controller::AI_CHASE_CONTROLLER:
			return new AIChaseController(controlledInstance);

		case Controller::AI_FLEE_CONTROLLER:
			return new AIFleeController(controlledInstance);

		default:
			return nullController;
		}
	}

	AIController::AIController(const PVInstance* controlledInstance)
		: SteppingController(controlledInstance),
		  retargetTime(0.0f)
	{
		for (int i = 0; i < Controller::NUM_INPUT_TYPES; i++)
		{
			buffer[i] = 0.0f;
		}

		buffer[Controller::CONSTANT_INPUT] = 1.0f;
	}

	AIController::~AIController() {}

	float AIController::getValue(Controller::InputType inputType) const
	{
		return buffer[inputType];
	}

	void AIChaseController::updateBuffer(float time)
	{
		updateTarget(Controller::AI_CHASE_CONTROLLER, time);

		if (!target.expired())
		{
			chaseTarget(getLocalTargetPosition());
		}
		else
		{
			buffer[Controller::LEFT_TRACK_INPUT] = -1.0f;
			buffer[Controller::RIGHT_TRACK_INPUT] = 1.0f;
			buffer[Controller::RIGHT_LEFT_INPUT] = 1.0f;
		}
	}

	SteppingController::SteppingController(const PVInstance* _controlledInstance)
		: runService(shared_from(ServiceProvider::create<RunService>(_controlledInstance))),
		  controlledInstance(shared_from(_controlledInstance))
	{
		if (runService)
			runService->Notifier<RunService, Heartbeat>::addListener(this);
	}

	SteppingController::~SteppingController()
	{
		if (runService)
			runService->Notifier<RunService, Heartbeat>::removeListener(this);
	}

	PlayerController::PlayerController(ControllerService* _controllerService, const PVInstance* _controlledInstance)
		: SteppingController(_controlledInstance),
		  controllerService(shared_from(_controllerService)),
		  stepsRotating(0)
	{
	}

	const UserInputBase* PlayerController::getHardwareDevice() const
	{
		RBXASSERT(!controllerService.expired());
		return controllerService.expired() ? NULL : controllerService.lock()->getHardwareDevice();
	}
}
