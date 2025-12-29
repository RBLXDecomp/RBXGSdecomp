#pragma once
#include "gui/Widget.h"
#include "gui/GuiDraw.h"

namespace RBX
{
	extern const char* sBackpackItem;
	class BackpackItem : public DescribedNonCreatable<BackpackItem, Widget, &sBackpackItem>
	{
	private:
		GuiDrawImage guiImageDraw;
		TextureId textureId;
	  
	private:
		bool inBackpack();
		virtual bool askSetParent(const Instance*) const;
		virtual bool askAddChild(const Instance*) const;
	protected:
		virtual G3D::Vector2 getSize() const;
		virtual void render2d(Adorn*);
		virtual bool isEnabled();
		int getBinId() const;
	public:
		void setTextureId(const TextureId&);
		const TextureId getTextureId() const;
		virtual bool drawEnabled() const;
		virtual bool drawSelected() const;
		virtual void onLocalClicked();
		virtual void onLocalOtherClicked();
	public:
		//BackpackItem(const BackpackItem&);
		BackpackItem();
		virtual ~BackpackItem();
	public:
		//BackpackItem& operator=(const BackpackItem&);
	};
}
