#include "gui/Gui.h"

namespace RBX
{
	const G3D::Color4& GuiItem::disabledFill()
	{
		static G3D::Color4 c(0.7f, 0.7f, 0.7f, 0.5f);
		return c;
	}

	const G3D::Color4& GuiItem::menuEnabledFill()
	{
		static G3D::Color4 c(0.3f, 0.3f, 0.3f, 1.0f);
		return c;
	}

	const G3D::Color4& GuiItem::menuDisabledFill()
	{
		static G3D::Color4 c(0.6f, 0.6f, 0.6f, 1.0f);
		return c;
	}

	const G3D::Color4& GuiItem::menuSelect()
	{
		static G3D::Color4 c(0.7f, 0.7f, 0.7f, 1.0f);
		return c;
	}

	const G3D::Color4& GuiItem::translucentBackdrop()
	{
		static G3D::Color4 c(0.6f, 0.6f, 0.6f, 0.6f);
		return c;
	}

	G3D::Vector2 GuiRoot::canvasSize;
	G3D::Matrix4 GuiRoot::oldProjectionMatrix;
	G3D::CoordinateFrame GuiRoot::oldCameraWorld;

	GuiItem::GuiItem() 
	{
		setName("Unnamed GuiItem");
	}

	const GuiItem* GuiItem::getGuiItem(int index) const
	{
		return dynamic_cast<const GuiItem*>(getChild(index));
	}

	GuiItem* GuiItem::getGuiItem(int index)
	{
		return dynamic_cast<GuiItem*>(getChild(index));
	}

	GuiResponse GuiItem::processNonFocus(const GuiEvent& event)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			GuiItem* current = getGuiItem((int)i);

			if (current && current != focus.get())
			{
				GuiResponse itemResponse = current->process(event);
				if (itemResponse.wasUsed())
				{
					this->loseFocus();

					focus = shared_from(current);
					focus->loseFocus();

					return itemResponse;
				}
			}
		}

		return GuiResponse::notUsed();
	}

	GuiResponse GuiItem::process(const GuiEvent& event)
	{
		if (event.isMouseEvent() && event.eventType == UIEvent::MOUSE_IDLE)
			return GuiResponse::notUsed();

		if (focus)
		{
			if (focus->canLoseFocus())
			{
				GuiResponse response = processNonFocus(event);
				if (response.wasUsed())
					return response;
			}

			GuiResponse focusResponse = focus->process(event);
			if (focusResponse.wasUsed())
				return focusResponse;

			loseFocus();
		}

		RBXASSERT(!focus);
		return processNonFocus(event);
	}

	void GuiItem::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		if (instance == focus)
			loseFocus();

		Instance::onDescendentRemoving(instance);
	}

	Rect GuiItem::getMyRect() const
	{
		G3D::Vector2 pos = getPosition();
		return Rect(pos, pos + getSize());
	}

	const GuiItem* GuiItem::getGuiParent() const
	{
		return dynamic_cast<const GuiItem*>(getParent());
	}

	void GuiItem::label2d(Adorn* adorn, const std::string& label, const G3D::Color4& fill, const G3D::Color4& border, Adorn::XAlign align) const
	{
		if (label.empty())
			return;

		Rect myRect = getMyRect();

		G3D::Vector2 size = myRect.size();
		G3D::Vector2 pos = myRect.center();
		switch (align)
		{
		case Adorn::XALIGN_LEFT:
		default:
			pos.x = myRect.low.x + size.x * 0.1f;
			break;

		case Adorn::XALIGN_RIGHT:
			pos.x = myRect.high.x - size.x * 0.1f;
			break;

		case Adorn::XALIGN_CENTER:
			break;
		}

		adorn->drawFont2D(label, pos, getFontSize(), fill, border, align, Adorn::YALIGN_CENTER, Adorn::PROPORTIONAL_SPACING);
	}

	void UnifiedWidget::init()
	{
		setGuiSize(G3D::Vector2(140.0f, 24.0f));
		menuState = NOTHING;
	}

	void UnifiedWidget::setMenuState(MenuState newMenuState)
	{
		if (menuState != newMenuState)
		{
			menuState = newMenuState;
			onMenuStateChanged();
		}
	}

	GuiResponse UnifiedWidget::processNothing(const GuiEvent& event)
	{
		RBXASSERT(menuState == NOTHING);

		loseFocus();

		Rect titleRect = getMyRect();
		if (titleRect.pointInRect(event.mousePosition))
		{
			if (event.eventType >= GuiEvent::MOUSE_LEFT_BUTTON_DOWN)
			{
				if (event.eventType > GuiEvent::MOUSE_LEFT_BUTTON_UP)
				{
					if (event.eventType == GuiEvent::MOUSE_MOVE)
					{
						setMenuState(HOVER);
						return GuiResponse::used();
					}
				}
				else
				{
					setMenuState(SHOWN_APPEARING);
					return GuiResponse::used();
				}
			}
		}
		else
		{
			setMenuState(NOTHING);
		}

		return GuiResponse::notUsed();
	}

	GuiResponse UnifiedWidget::processHover(const GuiEvent& event)
	{
		RBXASSERT(menuState == HOVER);

		loseFocus();

		Rect titleRect = getMyRect();
		if (titleRect.pointInRect(event.mousePosition))
		{
			switch (event.eventType)
			{
			case GuiEvent::MOUSE_LEFT_BUTTON_DOWN:
				setMenuState(SHOWN_APPEARING);
				return GuiResponse::used();

			case GuiEvent::MOUSE_LEFT_BUTTON_UP:
				setMenuState(SHOWN);
				return GuiResponse::used();

			default:
				return GuiResponse::used();
			}
		}
		else
		{
			setMenuState(NOTHING);
			return GuiResponse::notUsed();
		}
	}

	GuiResponse UnifiedWidget::processShown_InTitle(const GuiEvent& event)
	{
		loseFocus();

		if (menuState == SHOWN)
		{
			if (event.eventType == GuiEvent::MOUSE_LEFT_BUTTON_UP)
				setMenuState(HOVER);

			return GuiResponse::used();
		}
		else
		{
			RBXASSERT(menuState == SHOWN_APPEARING);
			if (event.eventType == GuiEvent::MOUSE_LEFT_BUTTON_UP)
				setMenuState(SHOWN);

			return GuiResponse::used();
		}
	}

	GuiResponse UnifiedWidget::processShown_OutOfTitle(const GuiEvent& event)
	{
		UnifiedWidget* oldFocus = dynamic_cast<UnifiedWidget*>(getFocus());

		if (event.eventType >= GuiEvent::MOUSE_LEFT_BUTTON_DOWN)
		{
			if (event.eventType > GuiEvent::MOUSE_LEFT_BUTTON_UP)
			{
				if (event.eventType == GuiEvent::MOUSE_MOVE)
				{
					MenuState oldFocusMenuState = oldFocus ? oldFocus->menuState : NOTHING;
					GuiResponse response = GuiItem::process(event);

					if (oldFocus)
					{
						if (UnifiedWidget* newFocus = dynamic_cast<UnifiedWidget*>(getFocus()))
						{
							if (newFocus != oldFocus)
								newFocus->setMenuState(oldFocusMenuState);
						}
					}

					return GuiResponse::used();
				}
			}
			else
			{
				GuiResponse response = GuiItem::process(event);

				if (response.wasNotUsed())
				{
					RBXASSERT(!getFocus());
					setMenuState(NOTHING);
				}
				else if (response.wasFinished())
				{
					loseFocus();
					setMenuState(NOTHING);
				}

				return response;
			}
		}

		RBXASSERT(false);
		return GuiResponse::notUsed();
	}

	GuiResponse UnifiedWidget::processShown(const GuiEvent& event)
	{
		Rect titleRect = getMyRect();
		if (titleRect.pointInRect(event.mousePosition))
			return processShown_InTitle(event);
		else
			return processShown_OutOfTitle(event);
	}

	GuiResponse UnifiedWidget::processKey(const GuiEvent& event)
	{
		return GuiResponse::notUsed();
	}

	GuiResponse UnifiedWidget::process(const GuiEvent& event)
	{
		if (isVisible())
		{
			if (event.isMouseEvent())
			{
				switch (menuState)
				{
				case NOTHING:
					return processNothing(event);
				case HOVER:
					return processHover(event);
				case SHOWN_APPEARING:
				case SHOWN:
					return processShown(event);
				default:
					return GuiResponse::notUsed();
				}
			}
			else
			{
				return processKey(event);
			}
		}
		
		return GuiResponse::notUsed();
	}

	void UnifiedWidget::onLoseFocus()
	{
		menuState = NOTHING;
	}

	G3D::Vector2 UnifiedWidget::childOffset() const
	{
		return G3D::Vector2(0.0f, 28.0f);
	}

	G3D::Vector2 UnifiedWidget::firstChildPosition() const
	{
		G3D::Vector2 pos = getPosition();
		G3D::Vector2 size = getSize();

		float x = pos.x + size.x + 4.0f;

		int sizeXMinus20 = (int)pos.y - 20;
		int sizeYPlus4 = (int)size.y + 4; 

		int desiredY = sizeXMinus20 / sizeYPlus4;
		int canvasY = (int)GuiRoot::getCanvasSize().y;

		int minY = (int)numChildren() / 2;
		int maxY = (canvasY - 120) / desiredY - minY;
		int y = G3D::min(maxY, G3D::max(desiredY, minY));
		y -= minY;
		y *= sizeYPlus4;
		y += 20;

		return G3D::Vector2(x, y);
	}

	G3D::Vector2 UnifiedWidget::getChildPosition(const GuiItem* child) const
	{
		G3D::Vector2 pos = firstChildPosition();

		for (size_t i = 0; i < numChildren(); ++i)
		{
			if (const GuiItem* guiItem = dynamic_cast<const GuiItem*>(getChild(i)))
			{
				if (guiItem == child)
					return pos;

				pos += childOffset();
			}
		}

		RBXASSERT(false);
		return pos;
	}

	void UnifiedWidget::render2dChildren(Adorn* adorn)
	{
		if (menuState >= SHOWN_APPEARING)
		{
			for (size_t i = 0; i < numChildren(); ++i)
			{
				if (GuiItem* guiItem = dynamic_cast<GuiItem*>(getChild(i)))
					guiItem->render2d(adorn);
			}
		}
	}

	void UnifiedWidget::render2dMe(Adorn* adorn)
	{
		adorn->rect2d(getMyRect2D(), menuState != NOTHING ? menuSelect() : G3D::Color3::white());
		adorn->outlineRect2d(getMyRect2D(), 1.0f, G3D::Color3::black());
		label2d(adorn, getName(), G3D::Color3::black(), G3D::Color4::clear(), Adorn::XALIGN_CENTER);
	}

	void UnifiedWidget::render2d(Adorn* adorn)
	{
		if (isVisible())
		{
			render2dMe(adorn);
			render2dChildren(adorn);
		}
	}

	TextDisplay::TextDisplay(const std::string& name, const std::string& label)
	{
		init();
		setName(name);
		setLabel(label);
	}

	void TextDisplay::init()
	{
		setGuiSize(G3D::Vector2(180.0f, 30.0f));
		fontSize = 18;
		fontColor = G3D::Color3::purple();
		borderColor = G3D::Color4::clear();
		align = Adorn::XALIGN_LEFT;
	}

	int TextDisplay::getFontSize() const
	{
		return fontSize;
	}

	std::string TextDisplay::getLabel() const
	{
		return label;
	}

	void TextDisplay::render2d(Adorn* adorn)
	{
		label2d(adorn, getLabel(), fontColor, borderColor, align);
	}

	TopMenuBar::TopMenuBar(const std::string& name, Layout::Style layoutStyle, bool useTranslucentBackdrop)
	{
		init();
		setName(name);
		this->layoutStyle = layoutStyle;

		if (useTranslucentBackdrop)
			backdropColor = translucentBackdrop();
	}

	void TopMenuBar::init()
	{
		layoutStyle = Layout::HORIZONTAL;
		backdropColor = G3D::Color4::clear();
		visible = true;
	}

	G3D::Vector2 TopMenuBar::getSize() const
	{
		G3D::Vector2 size(0.0f, 0.0f);

		for (size_t i = 0; i < numChildren(); i++)
		{
			const GuiItem* current = getGuiItem((int)i);

			if (current)
			{
				G3D::Vector2 childSize = current->getSize();
				switch (layoutStyle)
				{
				case Layout::VERTICAL:
					size.x = G3D::max(size.x, childSize.x);
					size.y += childSize.y;
					break;
				case Layout::HORIZONTAL:
					size.x += childSize.x;
					size.y = G3D::max(size.y, childSize.y);
					break;
				}
			}
		}

		return size;
	}

	void TopMenuBar::render2d(Adorn* adorn)
	{
		if (isVisible())
		{
			if (backdropColor != G3D::Color4::clear())
			{
				adorn->rect2d(getMyRect2D(), backdropColor);
			}

			for (size_t i = 0; i < numChildren(); i++)
			{
				GuiItem* current = getGuiItem((int)i);
				if (current)
					current->render2d(adorn);
			}
		}
	}

	GuiResponse TopMenuBar::process(const GuiEvent& event)
	{
		if (isVisible())
		{
			GuiResponse response = GuiItem::process(event);
			if (response.wasUsed())
			{
				return response;
			}

			if (backdropColor.a > 0.0f && event.isMouseEvent() && getMyRect().pointInRect(event.mousePosition))
			{
				return GuiResponse::used();
			}
		}

		return GuiResponse::notUsed();
	}

	void RelativePanel::init(const Layout& layout)
	{
		setName("RelativePanel");
		layoutStyle = layout.layoutStyle;
		backdropColor = layout.backdropColor;
		xLocation = layout.xLocation;
		yLocation = layout.yLocation;
		offset = layout.offset;
	}

	G3D::Vector2 PercentPanel::getPosition() const
	{
		Rect canvasRect(GuiRoot::getCanvasSize());
		G3D::Vector2 point = GuiRoot::toPixelSize(position);
		return canvasRect.positionPoint(point, xLocation, yLocation);
	}

	MenuSpacer::MenuSpacer(float x, float y)
	{
		setGuiSize(G3D::Vector2(x, y));
	}

	GuiRoot::GuiRoot() 
	{
		setName("GuiRoot");
	}

	int GuiRoot::normalizedFontSize(int fontSize)
	{
		static G3D::Vector2 percentSize(100.0f, 75.0f);
		return G3D::iRound(floorf(fontSize * toPixelSize(percentSize).x * 0.001f));
	}

	void GuiRoot::render2d(Adorn* adorn)
	{
		short height = static_cast<short>(adorn->getHeight());
		short width = static_cast<short>(adorn->getWidth());

		canvasSize = G3D::Vector2(width, height);

		for (size_t i = 0; i < numChildren(); i++)
		{
			GuiItem* current = getGuiItem((int)i);
			if (current)
				current->render2d(adorn);
		}
	}
}
