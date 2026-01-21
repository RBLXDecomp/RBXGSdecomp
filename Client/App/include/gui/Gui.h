#pragma once
#include "RbxGraphics/Adorn.h"
#include "v8tree/Instance.h"
#include "gui/GuiEvent.h"
#include "gui/Layout.h"
#include "util/Rect.h"
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
			return fastDynamicCast<const GuiItem>(instance) != NULL;
		}
		virtual const Name& getClassName() const
		{
			return Name::getNullName();
		}
	protected:
		GuiItem* getFocus();
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
		void label2d(Adorn*, const std::string&, const G3D::Color4&, const G3D::Color4&, Adorn::XAlign) const;

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
		virtual GuiResponse process(const GuiEvent&);
		virtual void render2d(Adorn* adorn)
		{
			return;
		}
	public:
		GuiItem(const GuiItem&);
		GuiItem();
	public:
		void addGuiItem(boost::shared_ptr<GuiItem>);
		void setGuiSize(const G3D::Vector2& guiSize)
		{
			this->guiSize = guiSize;
		}
		const G3D::Vector2& getGuiSize() const;
		__declspec(noinline) const GuiItem* getGuiParent() const;
		GuiItem* getGuiParent();
		const GuiItem* getGuiItem(int) const;
		GuiItem* getGuiItem(int);
	public:
		//GuiItem& operator=(const GuiItem&);

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

	class TopMenuBar : public GuiItem
	{
	protected:
		G3D::Color4 backdropColor;
		Layout::Style layoutStyle;
		bool visible;

	private:
		__declspec(noinline) void init();
	protected:
		virtual G3D::Vector2 getChildPosition(const GuiItem* child) const;
	public:
		//TopMenuBar(const TopMenuBar&);
		TopMenuBar(const std::string&, Layout::Style, G3D::Color4);
		TopMenuBar(const std::string&, Layout::Style, bool);
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
		void setVisible(bool);
	public:
		virtual ~TopMenuBar();
	public:
		//TopMenuBar& operator=(const TopMenuBar&);
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
		//RelativePanel(const RelativePanel&);
		RelativePanel(const Layout&);
		RelativePanel()
		{
			init(Layout());
		}
	public:
		virtual G3D::Vector2 getPosition() const;
	public:
		virtual ~RelativePanel();
	public:
		//RelativePanel& operator=(const RelativePanel&);
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
			return GuiRoot::getCanvasSize();
		}
	public:
		GuiRoot();
		virtual void render2d(Adorn*);
	protected:
		virtual bool askSetParent(const Instance*) const;
	
	private:
		static bool preProcess(const GuiEvent&);
	public:
		static GuiResponse processTarget(const GuiEvent&, GuiTarget*);
		static void render2dItem(Adorn*, GuiItem*);
		static void setCanvasSize(const G3D::Vector2&);
		static G3D::Vector2 getCanvasSize()
		{
			return canvasSize;
		}
		static G3D::Vector2 toPixelSize(const G3D::Vector2&);
		static int normalizedFontSize(int fontSize);
	};
}
