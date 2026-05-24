#pragma once
#include "util/Name.h"
#include "util/UIEvent.h"
#include "util/TextureId.h"
#include "util/IRenderable.h"

namespace RBX
{
	class PartInstance;
	class Primitive;
	class ContactManager;
	class ICameraOwner;
	class Surface;
	class HitTestFilter;
	class Workspace;
	class XmlState;

	class MouseCommand : public INamed, public IRenderable
	{
	private:
		bool capturedMouse;
	protected:
		Workspace* workspace;
		std::auto_ptr<XmlState> undoState;
	private:
		static G3D::Vector3 ignoreVector3;

	public:
		G3D::Ray getUnitMouseRay(const UIEvent&) const;
		G3D::Ray getSearchRay(const UIEvent&) const;
		PartInstance* getPart(const UIEvent&, const HitTestFilter*, G3D::Vector3&);
		PartInstance* getUnlockedPart(const UIEvent&, G3D::Vector3&);
		PartInstance* getPartByLocalCharacter(const UIEvent&, G3D::Vector3&);
		PartInstance* getUnlockedPartByLocalCharacter(const UIEvent&, G3D::Vector3&);
		Surface* getSurface(const UIEvent&, PartInstance*&, int&);
		Surface* getSurface(const UIEvent&);
		virtual MouseCommand* onMouseDown(const UIEvent&);
	protected:
		virtual void onMouseIdle(const UIEvent&);
		virtual void onMouseHover(const UIEvent&);
		virtual MouseCommand* onKeyDown(const UIEvent&);
		virtual MouseCommand* onPeekKeyDown(const UIEvent&);
		virtual void onMouseMove(const UIEvent&);
		virtual void onMouseDelta(const UIEvent&);
		virtual MouseCommand* onMouseUp(const UIEvent&);
		virtual void capture();
		virtual void releaseCapture();
		virtual void cancel();
		void snapshotSelectionPosition(XmlState*);
		bool characterCanReach(const G3D::Vector3&) const;
		MouseCommand(Workspace*);
	public:
		virtual ~MouseCommand();
		bool captured() const;
		virtual MouseCommand* isSticky() const;
		virtual TextureId getCursorId() const;
	private:
		virtual const std::string getCursorName() const;

	private:
		static const float maxSearch();
	public:
		static G3D::Ray getUnitMouseRay(UIEvent&, ICameraOwner*);
		static G3D::Ray getSearchRay(const G3D::Ray&);
		static G3D::Ray getSearchRay(const UIEvent&, ICameraOwner*);
		static Instance* getTopSelectable3d(PartInstance*);
		static PartInstance* getMousePart(const G3D::Ray&, const ContactManager&, const std::vector<const Primitive*>&, const HitTestFilter*, G3D::Vector3&, float);
		static PartInstance* getMousePart(const G3D::Ray&, const ContactManager&, const Primitive*, const HitTestFilter*, G3D::Vector3&, float);
	};
}
