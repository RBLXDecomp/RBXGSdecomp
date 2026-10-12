#include "v8datamodel/Surface.h"
#include "v8datamodel/PartInstance.h"
#include "v8xml/XmlElement.h"
#include "reflection/EnumConverter.h"

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

	void Surface::readValue(const XmlElement* element, IReferenceBinder& binder)
	{
		static const Name* desc_LegacyType = &Name::declare("Type", -1);
		static const Name* desc_LegacySurfaceInput = &Name::declare("SurfaceInput", -1);
		static const Name* desc_LegacyParamA = &Name::declare("ParamA", -1);
		static const Name* desc_LegacyParamB = &Name::declare("ParamB", -1);
		static const Name* desc_LegacyConstraint = &Name::declare("Constraint", -1);

		for (const XmlElement* iter = element->firstChild(); iter != NULL; iter = iter->nextSibling())
		{
			const Name* name = NULL;
			if (!iter->isXsiNil())
			{
				if (const XmlAttribute* attribute = iter->findAttribute(name_name))
				{
					if (attribute->getValue(name))
					{
						if (name == desc_LegacyType)
						{
							std::string text;
							if (iter->getValue(text))
							{
								SurfaceType value;
								if (Reflection::EnumDesc<SurfaceType>::singleton().convertToValue(text, value))
								{
									setSurfaceType(value);
								}
								else
								{
									setSurfaceType(NO_SURFACE);
								}
							}
						}
						else if (name == desc_LegacySurfaceInput)
						{
							std::string text;
							if (iter->getValue(text))
							{
								Controller::InputType value;
								if (Reflection::EnumDesc<Controller::InputType>::singleton().convertToValue(text, value))
								{
									setSurfaceInput(value);
								}
							}
						}
						else if (name == desc_LegacyParamA)
						{
							float value;
							if (iter->getValue(value))
							{
								setParamA(value);
							}
						}
						else if (name == desc_LegacyParamB)
						{
							float value;
							if (iter->getValue(value))
							{
								setParamB(value);
							}
						}
						else if (name == desc_LegacyConstraint)
						{
							std::string text;
							if (iter->getValue(text))
							{
								SurfaceType value;
								if (Reflection::EnumDesc<SurfaceType>::singleton().convertToValue(text, value))
								{
									if (value != NO_SURFACE)
									{
										setSurfaceType(value);
									}
								}
							}
						}
						else
						{
							RBXASSERT(false);
						}
					}
				}
			}
		}
	}
}
