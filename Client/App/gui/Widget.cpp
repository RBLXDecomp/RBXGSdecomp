#include "gui/Widget.h"

namespace RBX
{
	Widget::Widget()
		: widgetState(Widget::NOTHING)
	{
		setGuiSize(G3D::Vector2(100.0f, 25.0f));
	}

	void Widget::render2d(Adorn* adorn)
	{
		if (isVisible())
		{
			if (widgetState == Widget::HOVER || widgetState == Widget::DOWN_AWAY)
			{
				adorn->rect2d(getMyRect2D(), G3D::Color3::gray());
			}
			else if (widgetState == Widget::DOWN_OVER)
			{
				adorn->rect2d(getMyRect2D(), G3D::Color3::yellow());
			}

			label2d(adorn, getTitle(), isEnabled() ? getFontColor() : disabledFill(), G3D::Color4(0.5f, 0.5f, 0.5f, 0.25f), Adorn::XALIGN_LEFT);
		}
	}

	GuiResponse Widget::processMouse(const GuiEvent& event)
	{
		bool inWidget = getMyRect().pointInRect(event.mousePosition);

		switch (widgetState)
		{
		case NOTHING:
		case HOVER:
			if (inWidget)
			{
				if (!isEnabled())
				{
					widgetState = NOTHING;
					return GuiResponse::used();
				}

				switch (event.eventType)
				{
				case GuiEvent::MOUSE_LEFT_BUTTON_DOWN:
					widgetState = DOWN_OVER;
					onDown(event);
					return GuiResponse::used();

				case GuiEvent::MOUSE_MOVE:
					widgetState = HOVER;
					return GuiResponse::used();
					
				default:
					return GuiResponse::used();
				}
			}
			else
			{
				widgetState = NOTHING;
				return GuiResponse::notUsed();
			}

		case DOWN_OVER:
			switch (event.eventType)
			{
			case GuiEvent::MOUSE_LEFT_BUTTON_DOWN:
				return GuiResponse::used();

			case GuiEvent::MOUSE_LEFT_BUTTON_UP:
				widgetState = NOTHING;
				onUp(event);

				if (inWidget)
				{
					onClick(event);
					return GuiResponse::finished();
				}
				else
				{
					return GuiResponse::notUsed();
				}

			case GuiEvent::MOUSE_MOVE:
				if (!inWidget)
					widgetState = onDrag(event) ? NOTHING : DOWN_AWAY;

				return GuiResponse::used();

			default:
				return GuiResponse::notUsed();
			}

		case DOWN_AWAY:
			switch (event.eventType)
			{
			case GuiEvent::MOUSE_LEFT_BUTTON_DOWN:
				return GuiResponse::used();

			case GuiEvent::MOUSE_LEFT_BUTTON_UP:
				widgetState = NOTHING;
				onUp(event);

				if (inWidget)
				{
					onClick(event);
					return GuiResponse::used();
				}
				else
				{
					return GuiResponse::notUsed();
				}

			case GuiEvent::MOUSE_MOVE:
				widgetState = inWidget ? DOWN_OVER : DOWN_AWAY;
				return GuiResponse::used();
			}

		default:
			return GuiResponse::notUsed();
		}
	}

	GuiResponse Widget::processKey(const GuiEvent& event)
	{
		return GuiResponse::notUsed();
	}

	GuiResponse Widget::process(const GuiEvent& event)
	{
		if (isEnabled())
		{
			if (event.isMouseEvent())
				return processMouse(event);
			else
				return processKey(event);
		}
			
		return GuiResponse::notUsed();
	}

	VerbWidget::VerbWidget(Verb* verb)
		: verb(verb)
	{
	}

	VerbWidget::~VerbWidget()
	{
	}

	bool VerbWidget::isEnabled()	
	{
		return isVisible() && verb && verb->isEnabled();
	}

	void VerbWidget::onClick(const GuiEvent& event)
	{
		if (verb && verb->isEnabled())
			verb->doIt(event.iDataState);
	}
}
