#pragma once
#include "v8datamodel/MouseCommand.h"
#include "tool/MegaDragger.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sGroupDragTool;

	class GroupDragTool : public Named<MouseCommand, &sGroupDragTool>
	{
	private:
		std::auto_ptr<MegaDragger> megaDragger;
		G3D::Vector2 downPoint;
		bool dragging;
		G3D::Vector3 lastHit;

	public:
		virtual MouseCommand* onKeyDown(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseDown(const UIEvent& uiEvent);
		virtual void onMouseIdle(const UIEvent& uiEvent);
		virtual void onMouseMove(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseUp(const UIEvent& uiEvent);
		virtual const std::string getCursorName() const;

		GroupDragTool(PartInstance* mousePart, const G3D::Vector3& hitPointWorld, const std::vector<boost::weak_ptr<PartInstance>>& partArray, Workspace* workspace);
		virtual ~GroupDragTool();
	};
}
