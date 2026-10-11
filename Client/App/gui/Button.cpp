#include "gui/Button.h"

namespace RBX
{
	ImageWidget::ImageWidget(Adorn* adorn, const std::string& name)
		: guiImageDraw(adorn, name)
	{
		setName(name);
	}

	void ImageWidget::render2d(Adorn* adorn)
	{
		if (isVisible())
			guiImageDraw.render2d(adorn, isEnabled(), getMyRect(), Widget::NOTHING);
	}

	ImageButton::ImageButton(Adorn* adorn, Verb* verb, const std::string& name)
		: VerbWidget(verb),
		guiImageDraw(adorn, verb->getName().toString())
	{
		setName(name);
	}

	void ImageButton::render2d(Adorn* adorn)
	{
		if (isVisible())
			guiImageDraw.render2d(adorn, isEnabled(), getMyRect(), widgetState);
	}

	G3D::Vector2 ImageButton::getSize() const
	{
		return guiImageDraw.getImageSize();
	}

	KeyButton::KeyButton(const std::string& name, SDLKey key)
		: keyCode(key),
		  keyIsDown(false)
	{
		setName(name);
	}

	bool KeyButton::isEnabled()
	{
		return isVisible();
	}

	void KeyButton::onDown(const UIEvent& event)
	{
		event.userInput->setKeyState(keyCode, true);
	}

	void KeyButton::onUp(const UIEvent& event)
	{
		event.userInput->setKeyState(keyCode, false);
	}

	GuiResponse KeyButton::process(const GuiEvent& event)
	{
		if (event.eventType == GuiEvent::KEY_DOWN && event.key == keyCode)
			keyIsDown = true;
		if (event.eventType == GuiEvent::KEY_UP && event.key == keyCode)
			keyIsDown = false;

		return Widget::process(event);
	}

	ImageKeyButton::ImageKeyButton(Adorn* adorn, const std::string& name, SDLKey key)
		: KeyButton(name, key),
		  imageDraw(adorn, name)
	{
	}

	void ImageKeyButton::render2d(Adorn* adorn)
	{
		if (isVisible())
			imageDraw.render2d(adorn, isEnabled(), getMyRect(), keyIsDown ? DOWN_OVER : widgetState);
	}

	G3D::Vector2 ImageKeyButton::getSize() const
	{
		return imageDraw.getImageSize();
	}
}
