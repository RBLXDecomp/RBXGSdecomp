#pragma once
#include "v8tree/Service.h"
#include "gui/Widget.h"
#include "gui/GuiDraw.h"

namespace RBX
{
	class Hopper : public RelativePanel
	{
	protected:
		virtual bool askSetParent(const Instance* instance) const;
		virtual bool askAddChild(const Instance* instance) const;
	public:
		Hopper();
	};

	extern const char* sStarterPackService;
	class StarterPackService : public DescribedCreatable<StarterPackService, Hopper, &sStarterPackService>,
							   public Service
	{
	public:
		StarterPackService();
		virtual void render2d(Adorn* adorn);
	};

	extern const char* sLegacyHopperService;
	class LegacyHopperService : public DescribedCreatable<LegacyHopperService, Hopper, &sLegacyHopperService>,
								public Service
	{
	private:
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
	public:
		LegacyHopperService();
		virtual ~LegacyHopperService();
	};

	extern const char* sBackpackItem;
	class BackpackItem : public DescribedNonCreatable<BackpackItem, Widget, &sBackpackItem>
	{
	private:
		GuiDrawImage guiImageDraw;
		TextureId textureId;
	  
	private:
		bool inBackpack();
		virtual bool askSetParent(const Instance* instance) const;
		virtual bool askAddChild(const Instance* instance) const;
	protected:
		virtual G3D::Vector2 getSize() const;
		virtual void render2d(Adorn*);
		/*
			TODO:

			isEnabled is defined in the header, however defining it here causes problems,
			as doing so would require including Backpack.h in this header.

			This is not easily possible without doing some very hacky workarounds,
			as Hopper needs to be defined before Backpack is, and
			Backpack needs to be defined before BackpackItem is.
		*/
		virtual bool isEnabled();
		int getBinId() const;
	public:
		void setTextureId(const TextureId& value);
		const TextureId getTextureId() const;

		virtual bool drawEnabled() const
		{
			return true;
		}

		virtual bool drawSelected() const
		{
			return false;
		}

		virtual void onLocalClicked()
		{
			return;
		}

		virtual void onLocalOtherClicked()
		{
			return;
		}
	};

	extern const char* sHopperBin;
	class HopperBin : public DescribedCreatable<HopperBin, BackpackItem, &sHopperBin>
	{
	public:
		enum BinType
		{
			SCRIPT_BIN,
			GAME_TOOL,
			GRAB_TOOL,
			CLONE_TOOL,
			HAMMER_TOOL,
			SLINGSHOT_TOOL,
			ROCKET_TOOL,
			LASER_TOOL
		};

	private:
		BinType binType;
		bool active;
	  
	private:
		void onSelectScript();
		void onSelectCommand();
		int getCursor();
		virtual bool drawSelected() const
		{
			return active;
		}
	public:
		HopperBin();
	public:
		BinType getBinType() const
		{
			return binType;
		}

		void setBinType(const BinType value);
		virtual void onLocalClicked();
		virtual void onLocalOtherClicked();
		void setLegacyCommand(const std::string& text);
		void setLegacyTextureName(const std::string& value);
	};
}
