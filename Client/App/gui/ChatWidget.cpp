#include "gui/ChatWidget.h"
#include "util/Sound.h"
#include "Network/Players.h"

namespace RBX
{
	ChatWidget::ChatWidget(const std::string& name, std::string code)
	{
		setName(name);
		this->code = code;
	}

	void ChatWidget::onMenuStateChanged()
	{
		if (getMenuState() == HOVER)
			setMenuState(SHOWN);
	}

	GuiResponse ChatWidget::process(const GuiEvent& event)
	{
		if (isVisible())
		{
			if (event.isMouseEvent())
			{
				if (event.eventType == GuiEvent::MOUSE_LEFT_BUTTON_UP)
				{
					if (getMyRect().pointInRect(event.mousePosition))
					{
						if (Network::Players* players = ServiceProvider::find<Network::Players>(this))
						{
							Soundscape::SoundService* soundService = ServiceProvider::create<Soundscape::SoundService>(this);
							soundService->playSound(CLICK_SOUND);

							std::string message = "/sc " + code;
							players->chat(message);

							return GuiResponse::finished();
						}
					}
				}
			}
		}

		return UnifiedWidget::process(event);
	}

	UnifiedImageWidget::UnifiedImageWidget(Adorn* adorn, const std::string& name)
		: guiImageDraw(adorn, name)
	{
	}

	Widget::WidgetState UnifiedImageWidget::getWidgetState() const
	{
		switch (getMenuState())
		{
		case NOTHING:
			return Widget::NOTHING;
		case HOVER:
			return Widget::HOVER;
		case SHOWN_APPEARING:
			return Widget::DOWN_OVER;
		default:
			return Widget::NOTHING;
		}
	}

	void UnifiedImageWidget::render2dMe(Adorn* adorn)
	{
		if (isVisible())
		{
			guiImageDraw.render2d(adorn, true, getMyRect(), getWidgetState());
		}
	}

	ChatButton::ChatButton(Adorn* adorn, const std::string& name)
		: UnifiedImageWidget(adorn, name)
	{
	}
}
