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

			label2d(adorn, getTitle(), isEnabled() ? getFontColor() : GuiItem::disabledFill(), G3D::Color4(0.5f, 0.5f, 0.5f, 0.25f), Adorn::XALIGN_LEFT);
		}
	}
}
