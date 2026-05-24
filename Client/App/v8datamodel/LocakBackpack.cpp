#include "v8datamodel/LocalBackpack.h"

namespace RBX
{
	LocalBackpackItem::LocalBackpackItem() {}

	void LocalBackpackItem::onClick(const GuiEvent& event)
	{
		LocalBackpack* parent = static_cast<LocalBackpack*>(getParent());
		RBXASSERT(fastDynamicCast<LocalBackpack>(getParent()) == parent);

		parent->onClick(this);
	}

	G3D::Vector2 LocalBackpackItem::getSize() const
	{
		float width = GuiRoot::toPixelSize(G3D::Vector2(10.0f, 10.0f)).x;
		return G3D::Vector2(width, width);
	}

	LocalBackpack::LocalBackpack()
		: lastRemovedIndex(-1)
	{
		yLocation = Rect::BOTTOM;
	}

	LocalBackpack::~LocalBackpack()
	{
		clearAll();
	}

	void LocalBackpack::onEvent(const Instance* source, ChildAdded event)
	{
		Instance* child = event.child.get();

		if (Network::Player* player = fastDynamicCast<Network::Player>(child))
		{
			if (player == Network::Players::findLocalPlayer(this))
			{
				onLocalPlayerAdded(player);
			}
		}
		else
		{
			if (Backpack* backpack = fastDynamicCast<Backpack>(child))
			{
				onLocalBackpackAdded(backpack);
			}
			else if (BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(child))
			{
				insertBackpackItem(backpackItem);
			}
		}
	}

	void LocalBackpack::onEvent(const Instance* source, ChildRemoved event)
	{
		Instance* child = event.child.get();

		if (Network::Player* player = fastDynamicCast<Network::Player>(child))
		{
			if (player == localPlayer.get())
			{
				clearLocalPlayer();
			}
		}
		else
		{
			if (Backpack* backpack = fastDynamicCast<Backpack>(child))
			{
				RBXASSERT(localBackpack);
				clearLocalBackpack();
			}
			else if (BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(child))
			{
				removeBackpackItem(backpackItem);
			}
		}
	}

	void LocalBackpack::onEvent(const Instance* source, DescendentAdded event)
	{
		RBXASSERT(source == localCharacter.get());

		BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(event.instance.get());
		if (backpackItem)
			insertBackpackItem(backpackItem);
	}

	void LocalBackpack::onEvent(const Instance* source, DescendentRemoving event)
	{
		RBXASSERT(source == localCharacter.get());

		BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(event.instance.get());
		if (backpackItem)
			removeBackpackItem(backpackItem);
	}

	void LocalBackpack::onClick(LocalBackpackItem* clickedItem)
	{
		pendingClick = shared_from(clickedItem->getItem());
	}
}
