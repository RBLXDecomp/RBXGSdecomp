#include "gui/Gui.h"

namespace RBX
{
	const G3D::Color4& GuiItem::disabledFill()
	{
		static G3D::Color4 c(0.7f, 0.7f, 0.7f, 0.5f);
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

	GuiResponse GuiItem::processNonFocus(const GuiEvent& event)
	{
		for (size_t i = 0; i < numChildren(); i++)
		{
			GuiItem* current = fastDynamicCast<GuiItem>(getChild(i));

			if (current && current != focus.get())
			{
				GuiResponse itemResponse = current->process(event);
				if (!itemResponse.wasNotUsed())
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
		return fastDynamicCast<const GuiItem>(getParent());
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
				if (!response.wasNotUsed())
					return response;
			}

			GuiResponse focusResponse = focus->process(event);
			if (!focusResponse.wasNotUsed())
				return focusResponse;

			loseFocus();
		}

		RBXASSERT(!focus);
		return processNonFocus(event);
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
			const GuiItem* current = fastDynamicCast<const GuiItem>(getChild(i));

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
				GuiItem* current = fastDynamicCast<GuiItem>(getChild(i));
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
			if (!response.wasNotUsed())
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
			GuiItem* current = fastDynamicCast<GuiItem>(getChild(i));
			if (current)
				current->render2d(adorn);
		}
	}
}
