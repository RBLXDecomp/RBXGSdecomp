#pragma once
#include "gui/Gui.h"
#include "v8tree/Verb.h"

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
		GuiResponse processMouse(const GuiEvent& event);
		GuiResponse processKey(const GuiEvent& event);
	protected:
		virtual GuiResponse process(const GuiEvent& event);

		virtual void onLoseFocus()
		{
			widgetState = Widget::NOTHING;
		}

		virtual void render2d(Adorn* adorn);

		virtual void onClick(const GuiEvent& event)
		{
		}

		virtual bool onDrag(const GuiEvent& event)
		{
			return false;
		}

		virtual void onDown(const UIEvent& event)
		{
		}

		virtual void onUp(const UIEvent& event)
		{
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
		Widget();
	};

	class VerbWidget : public Widget
	{
	private:
		Verb* verb;

	protected:
		Verb* getVerb()
		{
			return verb;
		}
		virtual bool isEnabled();
		virtual void onClick(const GuiEvent& event);
	public:
		VerbWidget(Verb* verb);
		VerbWidget();
		virtual ~VerbWidget();
	};

	class MultiVerbWidget : public Widget
	{
	protected:
		virtual Verb* currentVerb();
		virtual bool isEnabled();
		virtual void onClick(const GuiEvent&);
	public:
		MultiVerbWidget(const std::string&);
		virtual ~MultiVerbWidget();
	};
}
