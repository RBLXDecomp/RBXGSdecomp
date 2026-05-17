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

		Network::Player* player = fastDynamicCast<Network::Player>(child);
		if (player)
		{
			if (player == Network::Players::findLocalPlayer(this))
			{
				onLocalPlayerAdded(player);
			}
		}
		else
		{
			Backpack* backpack = fastDynamicCast<Backpack>(child);
			if (backpack)
			{
				onLocalBackpackAdded(backpack);
			}
			else
			{
				BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(child);
				if (backpackItem)
					insertBackpackItem(backpackItem);
			}
		}
	}

	void LocalBackpack::onEvent(const Instance* source, ChildRemoved event)
	{
		Instance* child = event.child.get();

		Network::Player* player = fastDynamicCast<Network::Player>(child);
		if (player)
		{
			if (player == localPlayer.get())
			{
				clearLocalPlayer();
			}
		}
		else
		{
			Backpack* backpack = fastDynamicCast<Backpack>(child);
			if (backpack)
			{
				RBXASSERT(localBackpack);
				clearLocalBackpack();
			}
			else
			{
				BackpackItem* backpackItem = fastDynamicCast<BackpackItem>(child);
				if (backpackItem)
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
