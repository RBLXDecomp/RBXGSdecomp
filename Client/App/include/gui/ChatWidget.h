#pragma once
#include "gui/Widget.h"
#include "gui/GuiDraw.h"

namespace RBX
{
	class ChatWidget : public UnifiedWidget
	{
	private:
		std::string code;

	private:
		std::string findMenuString(GuiItem*);
		virtual void onMenuStateChanged();
		virtual GuiResponse process(const GuiEvent& event);
	public:
		ChatWidget(const std::string& name, std::string code);
	};

	class UnifiedImageWidget : public UnifiedWidget
	{
	protected:
		GuiDrawImage guiImageDraw;

	public:
		UnifiedImageWidget(Adorn* adorn, const std::string& name);
		Widget::WidgetState getWidgetState() const;
		virtual void render2dMe(Adorn* adorn);
		void setSize(const G3D::Vector2& size);
		virtual G3D::Vector2 getSize() const
		{
			return guiImageDraw.getImageSize();
		}
	};

	class ChatButton : public UnifiedImageWidget
	{
	private:
		virtual bool isVisible() const
		{
			return true;
		}
	public:
		ChatButton(Adorn* adorn, const std::string& name);
	};
}
