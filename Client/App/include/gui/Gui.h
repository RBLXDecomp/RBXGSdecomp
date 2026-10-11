#pragma once
#include "RbxGraphics/Adorn.h"
#include "v8tree/Instance.h"
#include "gui/GuiEvent.h"
#include "gui/Layout.h"
#include "util/Rect.h"
#include <G3D/Matrix4.h>
#include <G3D/Vector2.h>

namespace RBX
{
	class GuiItem : public Instance,
					public GuiTarget
	{
	private:
		boost::shared_ptr<GuiItem> focus;
		G3D::Vector2 guiSize;
	  
	private:
		GuiResponse processNonFocus(const GuiEvent& event);
		void switchFocus(GuiItem*);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance);
		virtual bool askAddChild(const Instance* instance) const
		{
			return dynamic_cast<const GuiItem*>(instance) != NULL;
		}
		virtual const Name& getClassName() const
		{
			return Name::getNullName();
		}
	protected:
		GuiItem* getFocus()
		{
			return focus.get();
		}
		void loseFocus()
		{
			if (focus)
				focus->onLoseFocus();

			focus.reset();
		}

		Rect getMyRect() const;
		G3D::Rect2D getMyRect2D() const
		{
			return getMyRect().toRect2D();
		}
	protected:
		void label2d(Adorn* adorn, const std::string& label, const G3D::Color4& fill, const G3D::Color4& border, Adorn::XAlign align) const;

		virtual void onLoseFocus()
		{
			return;
		}

		virtual bool canLoseFocus()
		{
			return false;
		}

		virtual G3D::Vector2 getPosition() const
		{
			return getGuiParent()->getChildPosition(this);
		}

		virtual G3D::Vector2 getChildPosition(const GuiItem* child) const
		{
			RBXASSERT(0);
			return G3D::Vector2::zero();
		}

		virtual int getFontSize() const
		{
			return 14;
		}

		virtual bool isVisible() const
		{
			return true;
		}

		virtual std::string getTitle()
		{
			return getName();
		}

	public:
		virtual G3D::Vector2 getSize() const
		{
			return guiSize;
		}
		virtual GuiResponse process(const GuiEvent& event);
		virtual void render2d(Adorn* adorn)
		{
			return;
		}
	public:
		GuiItem(const GuiItem&);
		GuiItem();
	public:
		void addGuiItem(boost::shared_ptr<GuiItem> guiItem)
		{
			guiItem->setParent(this);
		}
		void setGuiSize(const G3D::Vector2& guiSize)
		{
			this->guiSize = guiSize;
		}
		const G3D::Vector2& getGuiSize() const
		{
			return this->guiSize;
		}
		__declspec(noinline) const GuiItem* getGuiParent() const;
		GuiItem* getGuiParent();
		const GuiItem* getGuiItem(int index) const;
		GuiItem* getGuiItem(int index);

	public:
		static const G3D::Color4& enabledFill();
		static const G3D::Color4& disabledFill();
		static const G3D::Color4& menuEnabledFill();
		static const G3D::Color4& menuDisabledFill();
		static const G3D::Color4& translucentBorder();
		static const G3D::Color4& menuSelect();
		static const G3D::Color4& menuBackground();
		static const G3D::Color4& fontBorder();
		static const G3D::Color4& translucentBackdrop();
		static const G3D::Color4& toolboxColor();
	};
	
	class UnifiedWidget : public GuiItem
	{
	public:
		enum MenuState
		{
			NOTHING,
			HOVER,
			SHOWN_APPEARING,
			SHOWN
		};

	private:
		MenuState menuState;

	private:
		GuiResponse processShown_InTitle(const GuiEvent& event);
		GuiResponse processShown_OutOfTitle(const GuiEvent& event);
		GuiResponse processNothing(const GuiEvent& event);
		GuiResponse processHover(const GuiEvent& event);
		GuiResponse processShown(const GuiEvent& event);
		GuiResponse processKey(const GuiEvent& event);
		void render2dChildren(Adorn* adorn);
		virtual G3D::Vector2 getChildPosition(const GuiItem* child) const;
		virtual bool canLoseFocus()
		{
			return true;
		}
		virtual void onLoseFocus();
		virtual int getFontSize() const
		{
			return 10;
		}
		void init();
	protected:
		bool showChildren();
		virtual void onMenuStateChanged()
		{
		}
		virtual G3D::Vector2 firstChildPosition() const;
		virtual G3D::Vector2 childOffset() const;
		virtual void render2dMe(Adorn* adorn);
	public:
		UnifiedWidget(const std::string&);
		UnifiedWidget()
		{
			init();
		}
		MenuState getMenuState() const
		{
			return menuState;
		}
		void setMenuState(MenuState newMenuState);
		virtual GuiResponse process(const GuiEvent& event);
		virtual void render2d(Adorn* adorn);
	};

	class TextDisplay : public GuiItem
	{
	protected:
		std::string label;
		int fontSize;
		G3D::Color4 fontColor;
		G3D::Color4 borderColor;
		Adorn::XAlign align;

	private:
		void init();
	protected:
		virtual int getFontSize() const;
		virtual std::string getLabel() const;
	public:
		TextDisplay(const std::string& name, const std::string& label);
		TextDisplay();
		virtual void render2d(Adorn* adorn);
		void setLabel(const std::string& label)
		{
			this->label = label;
		}
		void setFontSize(int fontSize)
		{
			this->fontSize = fontSize;
			setGuiSize(G3D::Vector2(this->fontSize * 10.0f, this->fontSize * 2.0f));
		}
		void setFontColor(const G3D::Color4& fontColor)
		{
			this->fontColor = fontColor;
		}
		void setBorderColor(const G3D::Color4& borderColor)
		{
			this->borderColor = borderColor;
		}
		void setAlign(Adorn::XAlign align)
		{
			this->align = align;
		}
	};

	class TopMenuBar : public GuiItem
	{
	protected:
		G3D::Color4 backdropColor;
		Layout::Style layoutStyle;
		bool visible;

	private:
		void init();
	protected:
		virtual G3D::Vector2 getChildPosition(const GuiItem* child) const;
	public:
		TopMenuBar(const std::string&, Layout::Style, G3D::Color4);
		TopMenuBar(const std::string& name, Layout::Style layoutStyle, bool useTranslucentBackdrop = false);
		TopMenuBar()
		{
			init();
		}
	public:
		virtual GuiResponse process(const GuiEvent& event);
		virtual void render2d(Adorn* adorn);
		virtual G3D::Vector2 getSize() const;
		virtual bool isVisible() const
		{
			return visible;
		}
		void setVisible(bool visible)
		{
			this->visible = visible;
		}
	};

	class RelativePanel : public TopMenuBar
	{
	protected:
		Rect::Location xLocation;
		Rect::Location yLocation;
		G3D::Vector2int16 offset;
	  
	protected:
		void init(const Layout& layout);
	public:
		RelativePanel(const Layout&);
		RelativePanel()
		{
			init(Layout());
		}
	public:
		virtual G3D::Vector2 getPosition() const;
	};

	class PercentPanel : public TopMenuBar
	{
	protected:
		Rect::Location xLocation;
		Rect::Location yLocation;
		G3D::Vector2 position;

	public:
		PercentPanel(Rect::Location xLocation, Rect::Location yLocation, const G3D::Vector2 position)
			: xLocation(xLocation),
			  yLocation(yLocation),
			  position(position)
		{
		}
		virtual G3D::Vector2 getPosition() const;
	};

	class MenuSpacer : public GuiItem
	{
	protected:
		void addItem(GuiItem*);
	public:
		MenuSpacer(float x, float y);
	};

	class MenuDivider : public MenuSpacer
	{
	public:
		MenuDivider(float, float);
		MenuDivider(float);
		MenuDivider();
		virtual void render2d(Adorn*);
	};

	class GuiRoot : public GuiItem
	{
	private:
		static G3D::Vector2 canvasSize;
		static G3D::Matrix4 oldProjectionMatrix;
		static G3D::CoordinateFrame oldCameraWorld;

	private:
		virtual G3D::Vector2 getSize() const
		{
			return getCanvasSize();
		}
	public:
		GuiRoot();
		virtual void render2d(Adorn* adorn);
	protected:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}
	
	private:
		static bool preProcess(const GuiEvent&);
	public:
		static GuiResponse processTarget(const GuiEvent&, GuiTarget*);
		static void render2dItem(Adorn*, GuiItem*);
		static void setCanvasSize(const G3D::Vector2& size)
		{
			canvasSize = size;
		}
		static G3D::Vector2 getCanvasSize()
		{
			return canvasSize;
		}
		static G3D::Vector2 toPixelSize(const G3D::Vector2&);
		static int normalizedFontSize(int fontSize);
	};
}
