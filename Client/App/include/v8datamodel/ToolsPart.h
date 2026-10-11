#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/BrickColor.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	class PartTool : public MouseCommand
	{
	private:
		boost::shared_ptr<PartInstance> partInstance;

	protected:
		virtual void render3dAdorn(Adorn*);
		virtual void onMouseHover(const UIEvent&);
	public:
		PartTool(Workspace*);
	};

	class FillToolColor : public Notifier<FillToolColor, BrickColor>
	{
	private:
		BrickColor color;

	public:
		FillToolColor();
		BrickColor get() const;
		void set(const BrickColor&);
	};

	extern char* sFillTool;

	class FillTool : public Named<PartTool, &sFillTool>
	{
	public:
		static FillToolColor color;

	protected:
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual const std::string getCursorName() const;
	public:
		FillTool(Workspace*);
		virtual MouseCommand* isSticky() const;
	};

	extern char* sDropperTool;

	class DropperTool : public Named<PartTool, &sDropperTool>
	{
	protected:
		virtual MouseCommand* onMouseDown(const UIEvent&);
		virtual const std::string getCursorName() const;
	public:
		DropperTool(Workspace*);
	};

	extern char* sModelSetPrimaryPartTool;

	class ModelSetPrimaryPartTool : public Named<PartTool, &sModelSetPrimaryPartTool>
	{
	protected:
		virtual MouseCommand* onMouseDown(const UIEvent&);
	public:
		ModelSetPrimaryPartTool(Workspace*);
	};
};
