#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/Filters.h"
#include "v8world/ContactManager.h"
#include "v8world/Primitive.h"
#include "humanoid/Humanoid.h"

namespace RBX
{
	MouseCommand::MouseCommand(Workspace* workspace)
		: workspace(workspace),
		  capturedMouse(false)
	{
	}

	MouseCommand::~MouseCommand()
	{
		RBXASSERT(undoState.get() == NULL);
	}

	bool MouseCommand::characterCanReach(const G3D::Vector3& hitPoint) const
	{
		PartInstance* head = Humanoid::getLocalHeadFromContext(workspace);

		if (head)
			return (head->getCoordinateFrame().translation - hitPoint).magnitude() < 60.0;
		else
			return false;
	}

	TextureId MouseCommand::getCursorId() const
	{
		return TextureId::fromAssets("Textures\\" + getCursorName() + ".png");
	}

	PartInstance* MouseCommand::getMousePart(const G3D::Ray& unitRay, const ContactManager& contactManager, const std::vector<const Primitive*>& ignore, const HitTestFilter* filter, G3D::Vector3& hitPoint, float maxSearchGrid)
	{
		RBXASSERT(G3D::fuzzyEq(unitRay.direction.squaredMagnitude(), 1.0f));

		G3D::Ray searchRay = G3D::Ray::fromOriginAndDirection(unitRay.origin, unitRay.direction * 2048.0f); // MouseCommand::getSearchRay inline
		Primitive* primitive = contactManager.getHit(searchRay, &ignore, filter, hitPoint);

		return primitive ? PartInstance::fromPrimitive(primitive) : NULL;
	}

	PartInstance* MouseCommand::getMousePart(const G3D::Ray& unitRay, const ContactManager& contactManager, const Primitive* ignore, const HitTestFilter* filter, G3D::Vector3& hitPoint, float maxSearchGrid)
	{
		std::vector<const Primitive*> ignorePrims;
		ignorePrims.push_back(ignore);

		return getMousePart(unitRay, contactManager, ignorePrims, filter, hitPoint, maxSearchGrid);
	}

	G3D::Ray MouseCommand::getSearchRay(const G3D::Ray& unitRay)
	{
		RBXASSERT(unitRay.direction.isUnit());
		return G3D::Ray::fromOriginAndDirection(unitRay.origin, unitRay.direction * 2048.0f);
	}

	G3D::Ray MouseCommand::getSearchRay(const UIEvent& uiEvent, ICameraOwner* _workspace)
	{
		G3D::Ray unitRay = getUnitMouseRay(uiEvent, _workspace);
		return getSearchRay(unitRay);
	}

	G3D::Ray MouseCommand::getSearchRay(const UIEvent& uiEvent) const
	{
		return getSearchRay(uiEvent, workspace);
	}

	G3D::Ray MouseCommand::getUnitMouseRay(const UIEvent& uiEvent, ICameraOwner* _workspace)
	{
		return _workspace->getGCamera().worldRay(
			uiEvent.mousePosition.x, 
			uiEvent.mousePosition.y, 
			G3D::Rect2D::xyxy(0.0f, 0.0f, uiEvent.windowSize.x, uiEvent.windowSize.y)
		);
	}

	G3D::Ray MouseCommand::getUnitMouseRay(const UIEvent& uiEvent) const
	{
		return getUnitMouseRay(uiEvent, workspace);
	}

	PartInstance* MouseCommand::getPart(const UIEvent& uiEvent, const HitTestFilter* filter, G3D::Vector3& hitWorld)
	{
		G3D::Ray unitRay = getUnitMouseRay(uiEvent);
		ContactManager& contactManager = workspace->getWorld()->getContactManager();

		std::vector<const Primitive*> ignorePrims;
		static_cast<ICameraOwner*>(workspace)->getCameraIgnorePrimitives(ignorePrims);

		if (Camera* camera = workspace->getCamera())
		{
			if (ICharacterSubject* subject = dynamic_cast<ICharacterSubject*>(camera->getCameraSubject()))
			{
				subject->getCameraIgnorePrimitives(ignorePrims);
			}
		}

		PartInstance* part = getMousePart(unitRay, contactManager, ignorePrims, filter, hitWorld, 2048.0f);

		if (!part)
			hitWorld = unitRay.origin + unitRay.direction * 10000.0f;

		return part;
	}

	PartInstance* MouseCommand::getUnlockedPart(const UIEvent& uiEvent, G3D::Vector3& hitWorld)
	{
		Unlocked filter;
		return getPart(uiEvent, &filter, hitWorld);
	}

	PartInstance* MouseCommand::getPartByLocalCharacter(const UIEvent& uiEvent, G3D::Vector3& hitWorld)
	{
		PartByLocalCharacter filter(workspace);
		return getPart(uiEvent, &filter, hitWorld);
	}

	void MouseCommand::capture()
	{
		RBXASSERT(!workspace->getCurrentMouseCommand()->captured());
		capturedMouse = true;
	}
}
