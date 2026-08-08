#include "v8datamodel/LocalBackpack.h"

namespace RBX
{
	LocalBackpackItem::LocalBackpackItem() {}

	void LocalBackpackItem::onClick(const GuiEvent& event)
	{
		LocalBackpack* parent = rbx_static_cast<LocalBackpack*>(getParent());

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

	void LocalBackpack::onLocalCharacterAdded(Instance* added)
	{
		localCharacter = shared_from(added);

		Notifier<Instance, DescendentAdded>::connect(localCharacter, this);
		Notifier<Instance, DescendentRemoving>::connect(localCharacter, this);
	}

	void LocalBackpack::onLocalPlayerAdded(Network::Player* added)
	{
		RBXASSERT(numChildren() == 0);
		RBXASSERT(added);

		localPlayer = shared_from(added);

		Notifier<Instance, ChildAdded>::connect(localPlayer, this);
		Notifier<Instance, ChildRemoved>::connect(localPlayer, this);
		Notifier<Network::Player, Network::CharacterAdded>::connect(localPlayer, this);
		Notifier<Network::Player, Network::CharacterRemoving>::connect(localPlayer, this);
	}

	void LocalBackpack::onLocalBackpackAdded(Backpack* added)
	{
		RBXASSERT(numChildren() <= 1);
		RBXASSERT(added->getParent() == localPlayer.get());

		localBackpack = shared_from(added);

		Notifier<Instance, ChildAdded>::connect(localBackpack, this);
		Notifier<Instance, ChildRemoved>::connect(localBackpack, this);
	}

	void LocalBackpack::clearAll()
	{
		clearLocalCharacter();
		clearLocalPlayer();
		clearLocalBackpack();

		if (players)
		{
			Notifier<Instance, ChildAdded>::disconnect(players.get(), this);
			Notifier<Instance, ChildRemoved>::disconnect(players.get(), this);

			players.reset();
		}
	}

	void LocalBackpack::clearLocalCharacter()
	{
		if (localCharacter)
		{
			Notifier<Instance, DescendentAdded>::disconnect(localCharacter, this);
			Notifier<Instance, DescendentRemoving>::disconnect(localCharacter, this);

			localCharacter.reset();
		}
	}

	void LocalBackpack::clearLocalPlayer()
	{
		removeAllChildren();

		if (localPlayer)
		{
			Notifier<Instance, ChildAdded>::disconnect(localPlayer, this);
			Notifier<Instance, ChildRemoved>::disconnect(localPlayer, this);
			Notifier<Network::Player, Network::CharacterAdded>::disconnect(localPlayer, this);
			Notifier<Network::Player, Network::CharacterRemoving>::disconnect(localPlayer, this);

			localPlayer.reset();
		}
	}

	void LocalBackpack::clearLocalBackpack()
	{
		removeAllChildren();

		if (localBackpack)
		{
			Notifier<Instance, ChildAdded>::disconnect(localBackpack, this);
			Notifier<Instance, ChildRemoved>::disconnect(localBackpack, this);

			localBackpack.reset();
		}

		lastRemovedIndex = -1;
	}

	void LocalBackpack::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		if (oldProvider)
			clearAll();

		Instance::onServiceProvider(oldProvider, newProvider);

		if (newProvider)
		{
			players = shared_from(ServiceProvider::create<Network::Players>(this));

			Notifier<Instance, ChildAdded>::connect(players, this);
			Notifier<Instance, ChildRemoved>::connect(players, this);
		}
	}
}
