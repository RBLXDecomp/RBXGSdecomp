#pragma once
#include "gui/Gui.h"
#include "gui/Widget.h"
#include "util/TextureId.h"

namespace RBX
{
	class GuiDrawImage
	{
	private:
		TextureId currentTexture;
		G3D::ReferenceCountedPointer<TextureProxyBase> normal;
		G3D::ReferenceCountedPointer<TextureProxyBase> disable;
		G3D::ReferenceCountedPointer<TextureProxyBase> hover;
		G3D::ReferenceCountedPointer<TextureProxyBase> down;
		mutable G3D::Vector2 size;

	private:
		void draw(Adorn* adorn, G3D::ReferenceCountedPointer<TextureProxyBase> texture, const Rect& rect, const G3D::Color4& behind, const G3D::Color4& inFront);
		void setImageFromName(Adorn* adorn, const std::string& name);
	public:
		GuiDrawImage(Adorn* adorn, const std::string& name)
		{
			setImageFromName(adorn, name);
		}
		GuiDrawImage()
		{
		}
	public:
		void render2d(Adorn* adorn, bool enabled, Rect& rect, Widget::WidgetState state);
		void setImageSize(const G3D::Vector2&);
		G3D::Vector2 getImageSize() const;
		bool setImage(Adorn* adorn, const TextureId& textureId);
	};
}
