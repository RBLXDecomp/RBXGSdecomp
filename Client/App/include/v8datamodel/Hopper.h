#pragma once
#include "v8datamodel/LocalBackpack.h"
#include "v8tree/Service.h"

namespace RBX
{
	class Hopper : public RelativePanel
	{
	protected:
		virtual bool askSetParent(const Instance*) const;
		virtual bool askAddChild(const Instance*) const;
	public:
		//Hopper(const Hopper&);
		Hopper();
		virtual ~Hopper();
	public:
		//Hopper& operator=(const Hopper&);
	};

	extern const char* sStarterPackService;
	class StarterPackService : public DescribedCreatable<StarterPackService, Hopper, &sStarterPackService>,
							   public Service
	{
	public:
		//StarterPackService(const StarterPackService&);
		StarterPackService();
	public:
		virtual void render2d(Adorn*);
	public:
		virtual ~StarterPackService();
	public:
		//StarterPackService& operator=(const StarterPackService&);
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
		virtual bool drawSelected() const;
	public:
		//HopperBin(const HopperBin&);
		HopperBin();
	public:
		BinType getBinType() const;
		void setBinType(const BinType);
		virtual void onLocalClicked();
		virtual void onLocalOtherClicked();
		void setLegacyCommand(const std::string&);
		void setLegacyTextureName(const std::string&);
	public:
		virtual ~HopperBin();
	public:
		//HopperBin& operator=(const HopperBin&);
	};
}
