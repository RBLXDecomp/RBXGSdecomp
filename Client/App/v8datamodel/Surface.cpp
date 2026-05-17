#include "v8datamodel/Surface.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	Surface::Surface(PartInstance* partInstance, NormalId surfId)
		: partInstance(partInstance),
		  surfId(surfId)
	{
	}

	bool Surface::isControllable() const
	{
		Controller::InputType inputType = getInput();
		return inputType != Controller::NO_INPUT && inputType != Controller::CONSTANT_INPUT && inputType != Controller::SIN_INPUT;
	}

	SurfaceType Surface::getSurfaceType() const
	{
		RBXASSERT(partInstance->getPrimitive());
		return partInstance->getPrimitive()->getSurfaceType(surfId);
	}

	void Surface::setSurfaceType(SurfaceType type)
	{
		if (type != getSurfaceType())
		{
			partInstance->getPrimitive()->setSurfaceType(surfId, type);
			partInstance->onSurfaceChanged(surfId);
			partInstance->raisePropertyChanged(partInstance->getSurfaces().getSurfaceType(surfId));
		}
	}

	// the functions below don't match 100% most likely due to Primitive::getSurfaceData

	Controller::InputType Surface::getInput() const
	{
		RBXASSERT(partInstance->getPrimitive());
		return partInstance->getPrimitive()->getSurfaceData(surfId).inputType;
	}

	float Surface::getParamA() const
	{
		RBXASSERT(partInstance->getPrimitive());
		return partInstance->getPrimitive()->getSurfaceData(surfId).paramA;
	}

	float Surface::getParamB() const
	{
		RBXASSERT(partInstance->getPrimitive());
		return partInstance->getPrimitive()->getSurfaceData(surfId).paramB;
	}
}
