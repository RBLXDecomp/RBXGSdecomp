#pragma once
#include "gui/Widget.h"
#include "gui/GuiDraw.h"

namespace RBX
{
	class TextButton : public VerbWidget
	{
	protected:
		virtual bool canLoseFocus();
	public:
		TextButton(Verb*, const std::string&, int);
		TextButton();
		virtual G3D::Color4 getFontColor();
	};

	class ImageWidget : public Widget
	{
	private:
		GuiDrawImage guiImageDraw;

	public:
		ImageWidget(Adorn* adorn, const std::string& name);
		virtual void render2d(Adorn* adorn);
		virtual G3D::Vector2 getSize() const
		{
			return guiImageDraw.getImageSize();
		}
	};

	class ImageButton : public VerbWidget
	{
	protected:
		GuiDrawImage guiImageDraw;

	public:
		ImageButton(Adorn* adorn, Verb* verb, const std::string& name);
		virtual void render2d(Adorn* adorn);
		void setSize(const G3D::Vector2&);
		virtual G3D::Vector2 getSize() const;
	};

	class ImageToggleButton : public ImageButton
	{
	public:
		ImageToggleButton(Adorn*, Verb*, const std::string&);
		virtual void render2d(Adorn*);
	};

	class MultiImageButton : public MultiVerbWidget
	{
	private:
		std::map<Verb*, GuiDrawImage*> images;

	protected:
		virtual bool canLoseFocus();
		void addVerb(Adorn*, Verb*);
	public:
		MultiImageButton(const std::string&, const G3D::Vector2&);
		virtual ~MultiImageButton();
		virtual void render2d(Adorn*);
	};

	class KeyButton : public Widget
	{
		friend class ImageKeyButton;

	private:
		SDLKey keyCode;
		bool keyIsDown;

	protected:
		virtual void onDown(const UIEvent& event);
		virtual void onUp(const UIEvent& event);
		virtual bool isEnabled();
		virtual GuiResponse process(const GuiEvent& event);
	public:
		KeyButton(const std::string& name, SDLKey key);
	};

	class ImageKeyButton : public KeyButton
	{
	private:
		GuiDrawImage imageDraw;

	public:
		ImageKeyButton(Adorn* adorn, const std::string& name, SDLKey key);
		virtual void render2d(Adorn* adorn);
		virtual G3D::Vector2 getSize() const;
	};
}
