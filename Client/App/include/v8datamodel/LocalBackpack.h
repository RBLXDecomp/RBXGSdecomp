#pragma once
#include "v8datamodel/Hopper.h"
#include "Network/Player.h"

namespace RBX
{
	extern const char* sLocalBackpackItem;
	class LocalBackpackItem : public DescribedCreatable<LocalBackpackItem, Widget, &sLocalBackpackItem>
	{
	private:
		boost::shared_ptr<BackpackItem> myItem;
		GuiDrawImage guiImageDraw;

	private:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}

		virtual G3D::Vector2 getSize() const;
		virtual void render2d(Adorn*);
		virtual void onClick(const GuiEvent& event);
		int getBinId() const;
	public:
		LocalBackpackItem();

		void setItem(BackpackItem* item)
		{
			myItem = shared_from(item);
		}

		BackpackItem* getItem()
		{
			return myItem.get();
		}
	};

	extern const char* sLocalBackpack;
	class LocalBackpack : public DescribedCreatable<LocalBackpack, RelativePanel, &sLocalBackpack>,
						  public Service,
						  public Listener<Instance, ChildAdded>,
						  public Listener<Instance, ChildRemoved>,
						  public Listener<Instance, DescendentAdded>,
						  public Listener<Instance, DescendentRemoving>,
						  public Listener<Network::Player, Network::CharacterAdded>,
						  public Listener<Network::Player, Network::CharacterRemoving>
	{
	private:
		boost::shared_ptr<Network::Players> players;
		boost::shared_ptr<Network::Player> localPlayer;
		boost::shared_ptr<Backpack> localBackpack;
		boost::shared_ptr<Instance> localCharacter;
		boost::shared_ptr<BackpackItem> pendingClick;
		int lastRemovedIndex;

	private:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onEvent(const Network::Player* source, Network::CharacterRemoving event);
		virtual void onEvent(const Network::Player* source, Network::CharacterAdded event);
		virtual void onEvent(const Instance* source, DescendentRemoving event);
		virtual void onEvent(const Instance* source, DescendentAdded event);
		virtual void onEvent(const Instance* source, ChildRemoved event);
		virtual void onEvent(const Instance* source, ChildAdded event);
		void onLocalCharacterAdded(Instance*);
		void onLocalPlayerAdded(Network::Player*);
		void onLocalBackpackAdded(Backpack*);
		void clearAll();
		void clearLocalCharacter();
		void clearLocalPlayer();
		void clearLocalBackpack();
		void insertBackpackItem(BackpackItem*);
		void removeBackpackItem(BackpackItem*);
	public:
		LocalBackpack();
		virtual ~LocalBackpack();
		void onHeartbeat();
		void onClick(LocalBackpackItem* clickedItem);
		bool onKey(int);
		void cycleUp();
		void cycleDown();

		virtual XmlElement* write()
		{
			return NULL;
		}
	};
}
