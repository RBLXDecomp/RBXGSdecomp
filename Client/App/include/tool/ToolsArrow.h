#pragma once
#include <G3D/Vector2int16.h>
#include <set>
#include "v8datamodel/MouseCommand.h"
#include "v8tree/Service.h"
#include "util/Name.h"

namespace RBX
{
	class Selection;

	class ArrowToolBase : public MouseCommand
	{
	protected:
		bool overInstance;

	protected:
		virtual void onMouseIdle(const UIEvent&);
		virtual void onMouseHover(const UIEvent&);
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual const std::string getCursorName() const;
	public:
		ArrowToolBase(Workspace*);
	};

	extern const char* sArrowTool;

	class ArrowTool : public Named<ArrowToolBase, &sArrowTool>
	{
	public:
		ArrowTool(Workspace*);
		virtual MouseCommand* isSticky() const;
	};

	extern const char* sBoxSelectCommand;

	class BoxSelectCommand : public Named<MouseCommand, &sBoxSelectCommand>
	{
	private:
		ServiceClient<Selection> selection;
		bool reverseSelecting;
		G3D::Vector2int16 mouseDownView;
		G3D::Vector2int16 mouseCurrentView;
		std::set<Instance*> previousItemsInBox;

	private:
		void getMouseInstances(std::set<Instance*>&, const UIEvent&, G3D::Rect2D&);
		void selectAnd(const std::set<Instance*>&);
		void selectReverse(const std::set<Instance*>&);
	public:
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual void onMouseMove(const UIEvent&);
		virtual void render2d(Adorn*);
		BoxSelectCommand(Workspace*);
	};
}
