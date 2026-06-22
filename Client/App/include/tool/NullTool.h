#pragma once
#include "v8datamodel/MouseCommand.h"
#include "util/Name.h"

namespace RBX
{
	extern const char* sNewNullTool;

	class NewNullTool : public Named<MouseCommand, &sNewNullTool>
	{
	private:
		std::string cursor;
		bool hasWaypoint;
		G3D::Vector3 waypoint;

	private:
		virtual void onMouseIdle(const UIEvent& uiEvent);
		virtual void onMouseHover(const UIEvent& uiEvent);
		virtual MouseCommand* onMouseDown(const UIEvent& uiEvent);

		virtual const std::string getCursorName() const
		{
			return cursor;
		}

		virtual MouseCommand* isSticky() const
		{
			return new NewNullTool(workspace);
		}

		virtual MouseCommand* onMouseUp(const UIEvent& uiEvent);

		virtual bool shouldRender3dAdorn() const
		{
			return true;
		}

		virtual void render3dAdorn(Adorn* adorn);
	public:
		NewNullTool(Workspace* workspace);
		virtual ~NewNullTool();
	};

	extern const char* sNullTool;

	// this class is completely unused in RBXGS
	class NullTool : public Named<MouseCommand, &sNullTool>
	{
	private:
		virtual const std::string getCursorName() const;
	public:
		NullTool(Workspace*);
		virtual ~NullTool();
		virtual MouseCommand* isSticky() const;
		virtual MouseCommand* onMouseUp(const UIEvent&);
	};
}
