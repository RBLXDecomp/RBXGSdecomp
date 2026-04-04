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
		virtual GuiResponse process(const GuiEvent& event);

		virtual void onLoseFocus()
		{
			widgetState = Widget::NOTHING;
		}

		virtual void render2d(Adorn* adorn);

		virtual void onClick(const GuiEvent&)
		{
			return;
		}

		virtual bool onDrag(const GuiEvent& event)
		{
			return false;
		}

		virtual void onDown(const UIEvent& event)
		{
			return;
		}

		virtual void onUp(const UIEvent&)
		{
			return;
		}

		virtual int getFontSize() const
		{
			return 12;
		}

		virtual G3D::Color4 getFontColor()
		{
			return G3D::Color3::white();
		}

		virtual bool isEnabled()
		{
			return isVisible();
		}

	public:
		//Widget(const Widget&);
		Widget();
		virtual ~Widget();
	public:
		//Widget& operator=(const Widget&);
	};
}
