#pragma once
#include "v8datamodel/MouseCommand.h"
#include "util/Name.h"

namespace RBX
{
	class RunDragger;
	class MegaDragger;

	extern const char* sPartDragTool;

	class PartDragTool : public Named<MouseCommand, &sPartDragTool>
	{
	private:
		std::auto_ptr<RunDragger> runDragger;
		std::auto_ptr<MegaDragger> megaDragger;
		G3D::Vector2 downPoint;
		bool dragging;
		G3D::Vector3 hitWorld;

	public:
		virtual MouseCommand* onMouseDown(const UIEvent& uiEvent);
		virtual void onMouseIdle(const UIEvent& uiEvent);
		virtual void onMouseMove(const UIEvent&);
		virtual void onMouseDelta(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseUp(const UIEvent& uiEvent);
		virtual MouseCommand* onKeyDown(const UIEvent& uiEvent);
		virtual void render3dAdorn(Adorn* adorn);

		virtual const std::string getCursorName() const
		{
			return dragging ? "GrabRotateCursor" : "DragCursor";
		}

		PartDragTool(PartInstance* mousePart, const G3D::Vector3& hitPointWorld, Workspace* workspace);
		virtual ~PartDragTool();
	};
}
