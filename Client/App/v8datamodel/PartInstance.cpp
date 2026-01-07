#include "v8datamodel/PartInstance.h"
#include <G3D/CoordinateFrame.h>
#include "reflection/type.h"

namespace RBX
{
	const char* const category_Part = "Part";

	static const Reflection::PropDescriptor<PartInstance, G3D::Vector3> prop_PositionUi("Position", "Data", &PartInstance::getTranslationUi, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, G3D::Vector3> prop_Velocity("Velocity", "Data", &PartInstance::getLinearVelocity, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, G3D::Vector3> prop_SizeUi("Size", category_Part, &PartInstance::getPartSizeUi, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, bool> prop_Dragging("DraggingV1", "Behavior", &PartInstance::getDragging, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, PartInstance::FormFactor> prop_formFactor("FormFactor", category_Part, &PartInstance::getFormFactor, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, float> prop_Friction("Friction", category_Part, &PartInstance::getFriction, NULL, Reflection::PropertyDescriptor::LEGACY);
	static const Reflection::PropDescriptor<PartInstance, float> prop_Elasticity("Elasticity", category_Part, &PartInstance::getElasticity, NULL, Reflection::PropertyDescriptor::LEGACY);

	namespace Reflection
	{
		// TODO: check if type singletons are matching
		template<>
		const Type& Type::singleton<G3D::CoordinateFrame>()
		{
			static Type type("CoordinateFrame", typeid(G3D::CoordinateFrame));
			return type;
		}
	}

	Surfaces& PartInstance::getSurfaces()
	{
		return surfaces;
	}

	const Surfaces& PartInstance::getSurfaces() const
	{
		return surfaces;
	}

	void PartInstance::onGuidChanged()
	{
		RBXASSERT(!myWorld);
		primitive->setGuid(getGuid());
	}

	bool PartInstance::nonNullInWorkspace(boost::shared_ptr<PartInstance> part)
	{
		bool null = part.get() && part->myWorld;
		RBXASSERT(!null || part->primitive->inPipeline());
		return null;
	}

	bool PartInstance::computeSurfacesNeedAdorn() const
	{
		for (int i = 0; i < 6; i++)
		{
			Surface& current = surfaces[(NormalId)i];
			if (current.getSurfaceType() == ROTATE || current.getSurfaceType() == ROTATE_P || current.getSurfaceType() == ROTATE_V)
				return true;
		}
		return false;
	}

	void PartInstance::legacyTraverseState(const G3D::CoordinateFrame& parentState)
	{
		RBXASSERT(legacyOffset.get());

		G3D::CoordinateFrame myState = parentState * *legacyOffset;
		primitive->setGridCorner(myState);
	}

	bool PartInstance::isControllable() const
	{
		for (int i = 0; i < 6; i++)
		{
			Surface& current = surfaces[(NormalId)i];
			if ((current.getSurfaceType() == ROTATE_P || current.getSurfaceType() == ROTATE_V) && current.isControllable())
				return true;
		}
		return false;
	}

	void PartInstance::onSurfaceChanged(RBX::NormalId surfId)
	{
		PersistentPart.setDirty();
		IsControllable.setDirty();
		SurfacesNeedAdorn.setDirty();
		shouldRenderSetDirty();
	}

	void PartInstance::setPhysics(const G3D::CoordinateFrame& value)
	{
		if (value != getCoordinateFrame())
		{
			RBXASSERT(Math::isOrthonormal(value.rotation));
			primitive->setCoordinateFrame(value);
			PersistentPart.setDirty();
			onExtentsChanged();
		}
	}

	void PartInstance::setPhysics(const G3D::CoordinateFrame& cf, const Velocity& vel)
	{
		primitive->getBody()->setVelocity(vel);
		setPhysics(cf);
	}

	const G3D::CoordinateFrame& PartInstance::getCoordinateFrame() const
	{
		return primitive->getCoordinateFrame();
	}

	const G3D::Vector3& PartInstance::getTranslationUi() const
	{
		return primitive->getBody()->getCoordinateFrame().translation;
	}

	float PartInstance::getMass()
	{
		return primitive->getBody()->getMass();
	}

	const Velocity& PartInstance::getVelocity() const
	{
		return primitive->getBody()->getVelocity();
	}

	const G3D::Vector3& PartInstance::getLinearVelocity() const
	{
		return getVelocity().linear;
	}

	const G3D::Vector3& PartInstance::getRotationalVelocity() const
	{
		return getVelocity().rotational;
	}

	const G3D::Vector3& PartInstance::getPartSizeXml() const
	{
		return primitive->getGeometry()->getGridSize();
	}

	G3D::Vector3 PartInstance::getPartSizeUi() const
	{
		return getPartSizeXml();
	}

	bool PartInstance::getDragging() const
	{
		return primitive->getDragging();
	}

	bool PartInstance::getCanCollide() const
	{
		return !primitive->getDragging() && primitive->getCanCollide() ? true : false;
	}

	bool PartInstance::getAnchored() const
	{
		return primitive->getAnchor();
	}

	float PartInstance::getFriction() const
	{
		return primitive->getFriction();
	}

	float PartInstance::getElasticity() const
	{
		return primitive->getElasticity();
	}

	Extents PartInstance::getExtentsWorld() const
	{
		return primitive->getExtentsWorld();
	}

	Extents PartInstance::getExtentsLocal() const
	{
		return primitive->getExtentsLocal();
	}

	bool PartInstance::shouldRender3dAdorn() const
	{
		return partType == Part::CYLINDER_PART ||
			   SurfacesNeedAdorn || 
			   highlightSleepParts || 
			   highlightAwakeParts || 
			   showPartCoord || 
			   showAnchoredParts ||
			   showUnalignedParts ||
			   showSpanningTree;
	}

	bool PartInstance::getLocked(Instance* instance)
	{
		PartInstance* thisPart = fastDynamicCast<PartInstance>(instance);

		if (thisPart)
		{
			return thisPart->locked;
		}
		else
		{
			for (size_t i = 0; i < instance->numChildren(); i++)
			{
				if (getLocked(instance->getChild(i)))
					return true;
			}
			return false;
		}
	}

	void PartInstance::onParentControllerChanged()
	{
		PVInstance::onParentControllerChanged();
		PersistentPart.setDirty();
		primitive->setController(getTopPVController());
	}

	void PartInstance::onCanAggregateChanged(bool canAggregate)
	{
		RBXASSERT(canAggregate == getCanAggregate());
		Notifier<PartInstance, CanAggregateChanged>::raise(canAggregate);
		shouldRenderSetDirty();
	}

	void PartInstance::onTouchThisStep(boost::shared_ptr<PartInstance> other)
	{
		event_Touched.fire(this, other);
	}

	void PartInstance::setCoordinateFrame(const G3D::CoordinateFrame& value)
	{
		if (value != getCoordinateFrame())
		{
			G3D::CoordinateFrame newValue = value;
			Math::orthonormalizeIfNecessary(newValue.rotation);
			setPhysics(newValue);

			raisePropertyChanged(prop_CFrame);
			raisePropertyChanged(prop_PositionUi);
		}
	}

	void PartInstance::setPartSizeXml(const G3D::Vector3& rbxSize)
	{
		if (rbxSize != getPartSizeXml())
		{
			G3D::Vector3 newRbxSize = partType == Part::BLOCK_PART ? rbxSize : rbxSize.xxx();
			RBXASSERT(newRbxSize.max(G3D::Vector3(1.0f, 0.4f, 1.0f)) == newRbxSize);

			if (legacyOffset.get())
			{
				G3D::CoordinateFrame oldCorner = primitive->getGridCorner();
				primitive->setGridSize(newRbxSize);
				primitive->setGridCorner(oldCorner);
			}
			else
			{
				primitive->setGridSize(newRbxSize);
			}

			raisePropertyChanged(prop_Size);
			raisePropertyChanged(prop_SizeUi);
			PersistentPart.setDirty();
			onExtentsChanged();
		}
	}

	void PartInstance::setDragging(bool value)
	{
		if (value != getDragging())
		{
			primitive->setDragging(value);
			raisePropertyChanged(prop_Dragging);
		}
	}

	void PartInstance::setRenderImportance(float value)
	{
		if (value != getRenderImportance())
		{
			renderImportance = value;
			raisePropertyChanged(prop_RenderImportance);
		}
	}

	void PartInstance::setCanCollide(bool value)
	{
		if (value != getCanCollide())
		{
			primitive->setCanCollide(value);
			raisePropertyChanged(prop_CanCollide);
		}
	}

	void PartInstance::setAnchored(bool value)
	{
		if (value != getAnchored())
		{
			primitive->setAnchor(value);
			raisePropertyChanged(prop_Anchored);
		}
	}

	void PartInstance::setPartLocked(bool value)
	{
		if (value != getPartLocked())
		{
			locked = value;
			raisePropertyChanged(prop_Locked);
		}
	}

	void PartInstance::setTransparency(float value)
	{
		if (value != getTransparencyXml())
		{
			transparency = value;
			raisePropertyChanged(prop_Transparency);
		}
	}

	void PartInstance::setAlphaModifier(float value)
	{
		if (value != alphaModifier)
		{
			alphaModifier = value;
			raisePropertyChanged(prop_Transparency);
		}
	}

	void PartInstance::setReflectance(float value)
	{
		if (value != getReflectance())
		{
			reflectance = value;
			raisePropertyChanged(prop_Reflectance);
		}
	}

	void PartInstance::setColor(BrickColor value)
	{
		if (value != color)
		{
			color = value;
			raisePropertyChanged(prop_BrickColor);
			raisePropertyChanged(prop_Color);
		}
	}

	void PartInstance::setFormFactorXml(FormFactor value)
	{
		if (value != getFormFactor())
		{
			formFactor = value;
			raisePropertyChanged(prop_formFactor);
		}
	}

	void PartInstance::setFriction(float friction)
	{
		float newFriction = G3D::clamp(friction, 0.0f, 2.0f);
		if (newFriction != getFriction())
		{
			primitive->setFriction(newFriction);
			raisePropertyChanged(prop_Friction);
		}
	}

	void PartInstance::setElasticity(float elasticity)
	{
		float newElasticity = G3D::clamp(elasticity, 0.0f, 1.0f);
		if (newElasticity != getElasticity())
		{
			primitive->setElasticity(newElasticity);
			raisePropertyChanged(prop_Elasticity);
		}
	}

	bool PartInstance::reportTouches() const
	{
		return !event_Touched.empty(this);
	}
}
