#pragma once
#include "gui/Gui.h"

namespace RBX
{
	class Widget : public GuiItem
	{
	public:
		enum WidgetState
		{
			NOTHING,
			HOVER,
			DOWN_OVER,
			DOWN_AWAY
		};

	protected:
		WidgetState widgetState;
	  
	private:
		GuiResponse processMouse(const GuiEvent&);
		GuiResponse processKey(const GuiEvent&);
	protected:
		virtual GuiResponse process(const GuiEvent&);
		virtual void onLoseFocus();
		virtual void render2d(Adorn*);
		virtual void onClick(const GuiEvent&);
		virtual bool onDrag(const GuiEvent&);
		virtual void onDown(const UIEvent&);
		virtual void onUp(const UIEvent&);
		virtual int getFontSize() const;
		virtual G3D::Color4 getFontColor();
		virtual bool isEnabled();
	public:
		//Widget(const Widget&);
		Widget();
		virtual ~Widget();
	public:
		//Widget& operator=(const Widget&);
	};
}
