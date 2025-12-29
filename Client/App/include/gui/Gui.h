#pragma once
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
		GuiResponse processNonFocus(const GuiEvent&);
		void switchFocus(GuiItem*);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);
		virtual bool askAddChild(const Instance*) const;
		virtual const Name& getClassName() const;
	protected:
		GuiItem* getFocus();
		void loseFocus();
		Rect getMyRect() const;
		G3D::Rect2D getMyRect2D() const;
	protected:
		void label2d(Adorn*, const std::string&, const G3D::Color4&, const G3D::Color4&, Adorn::XAlign) const;
		virtual void onLoseFocus();
		virtual bool canLoseFocus();
		virtual G3D::Vector2 getPosition() const;
		virtual G3D::Vector2 getChildPosition(const GuiItem*) const;
		virtual int getFontSize() const;
		virtual bool isVisible() const;
		virtual std::string getTitle();
	public:
		virtual G3D::Vector2 getSize() const;
		virtual GuiResponse process(const GuiEvent&);
		virtual void render2d(Adorn*);
	public:
		GuiItem(const GuiItem&);
		GuiItem();
	public:
		void addGuiItem(boost::shared_ptr<GuiItem>);
		void setGuiSize(const G3D::Vector2&);
		const G3D::Vector2& getGuiSize() const;
		const GuiItem* getGuiParent() const;
		GuiItem* getGuiParent();
		const GuiItem* getGuiItem(int) const;
		GuiItem* getGuiItem(int);
	public:
		virtual ~GuiItem();
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
		void init();
	protected:
		virtual G3D::Vector2 getChildPosition(const GuiItem*) const;
	public:
		//TopMenuBar(const TopMenuBar&);
		TopMenuBar(const std::string&, Layout::Style, G3D::Color4);
		TopMenuBar(const std::string&, Layout::Style, bool);
		TopMenuBar();
	public:
		virtual GuiResponse process(const GuiEvent&);
		virtual void render2d(Adorn*);
		virtual G3D::Vector2 getSize() const;
		virtual bool isVisible() const;
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
		void init(const Layout&);
	public:
		//RelativePanel(const RelativePanel&);
		RelativePanel(const Layout&);
		RelativePanel();
	public:
		virtual G3D::Vector2 getPosition() const;
	public:
		virtual ~RelativePanel();
	public:
		//RelativePanel& operator=(const RelativePanel&);
	};
}
