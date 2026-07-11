#include "v8datamodel/Hopper.h"
#include "v8datamodel/Backpack.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/Mouse.h"
#include "v8datamodel/DataModel.h"
#include "v8datamodel/ScriptMouseCommand.h"

namespace RBX
{
	static Reflection::PropDescriptor<BackpackItem, TextureId> desc_TextureId("TextureId", "Data", &BackpackItem::getTextureId, &BackpackItem::setTextureId, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::PropDescriptor<HopperBin, std::string> desc_legacyCommand("Command", "Data", NULL, &HopperBin::setLegacyCommand, Reflection::PropertyDescriptor::LEGACY);
	static Reflection::PropDescriptor<HopperBin, std::string> desc_legacyTextureName("TextureName", "Data", NULL, &HopperBin::setLegacyTextureName, Reflection::PropertyDescriptor::LEGACY);
	static Reflection::EnumPropDescriptor<HopperBin, HopperBin::BinType> desc_BinType("BinType", "Data", &HopperBin::getBinType, &HopperBin::setBinType, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::SignalDesc<HopperBin, void(void)> desc_Deselected("Deselected");
	static Reflection::SignalDesc<HopperBin, void(boost::shared_ptr<Instance>)> desc_Selected("Selected", "mouse");

	Hopper::Hopper()
	{
		yLocation = Rect::BOTTOM;
	}

	bool Hopper::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const BackpackItem>(instance) != NULL;
	}

	bool Hopper::askSetParent(const Instance* instance) const
	{
		return true;
	}

	bool BackpackItem::askSetParent(const Instance* instance) const
	{
		return true;
	}

	bool BackpackItem::askAddChild(const Instance* instance) const
	{
		return true;
	}

	const TextureId BackpackItem::getTextureId() const
	{
		return textureId;
	}

	bool BackpackItem::isEnabled()
	{
		return getParent() && fastDynamicCast<Backpack>(getParent()) != NULL;
	}

	void BackpackItem::setTextureId(const TextureId& value)
	{
		if (value != textureId)
		{
			textureId = value;

			std::string filename;
			ContentProvider::singleton().requestContentFile(value, filename);

			Workspace* workspace = ServiceProvider::find<Workspace>(this);
			if (workspace)
				workspace->raiseDrawChanged();

			raisePropertyChanged(desc_TextureId);
		}
	}

	G3D::Vector2 BackpackItem::getSize() const
	{
		float width = GuiRoot::toPixelSize(G3D::Vector2(10.0f, 10.0f)).x;
		return G3D::Vector2(width, width);
	}

	HopperBin::HopperBin()
		: binType(SCRIPT_BIN),
		  active(false)
	{
		setName("HopperBin");
	}

	void HopperBin::setBinType(const HopperBin::BinType value)
	{
		if (value != binType)
		{
			binType = value;
			raisePropertyChanged(desc_BinType);

			if (binType != SCRIPT_BIN)
			{
				std::string textureName = Reflection::EnumDesc<BinType>::singleton().convertToString(binType);
				setLegacyTextureName(textureName);
			}
		}
	}

	void HopperBin::setLegacyTextureName(const std::string& value)
	{
		TextureId textureId = ContentId::fromAssets("Textures\\" + value + ".png");
		setTextureId(textureId);
	}

	void HopperBin::setLegacyCommand(const std::string& text)
	{
		BinType newBinType;
		
		if (!Reflection::EnumDesc<BinType>::singleton().convertToValue(text, newBinType))
		{
			newBinType = SCRIPT_BIN;
		}
		
		setBinType(newBinType);
	}

	void HopperBin::onLocalClicked()
	{
		RBXASSERT(isEnabled());

		if (!active)
		{
			active = true;

			if (binType == SCRIPT_BIN)
			{
				onSelectScript();
			}
			else
			{
				onSelectCommand();
			}
		}
		else
		{
			onLocalOtherClicked();
		}
	}

	void HopperBin::onLocalOtherClicked()
	{
		if (active)
		{
			if (binType == SCRIPT_BIN)
				desc_Deselected.fire(this);

			active = false;

			Workspace* workspace = ServiceProvider::find<Workspace>(this);
			if (workspace)
				workspace->setDefaultMouseCommand();
		}
	}

	void HopperBin::onSelectScript()
	{
		Workspace* workspace = ServiceProvider::find<Workspace>(this);
		if (workspace)
		{
			ScriptMouseCommand* command = new ScriptMouseCommand(workspace);
			workspace->setMouseCommand(command);
			desc_Selected.fire(this, command->getMouse());
		}
	}

	void HopperBin::onSelectCommand()
	{
		RBXASSERT(binType != SCRIPT_BIN);

		DataModel* root = rbx_static_cast<DataModel*>(getRootAncestor());

		std::string command = Reflection::EnumDesc<BinType>::singleton().convertToString(binType) + "Tool";

		Verb* verb = static_cast<DataModel*>(root)->getVerb(command);
		RBXASSERT(verb);

		if (verb->isEnabled())
		{
			Workspace* workspace = ServiceProvider::find<Workspace>(this);
			if (workspace)
				verb->doIt(&workspace->getDataState());
			else
				RBXASSERT(0);
		}
	}

	StarterPackService::StarterPackService()
	{
		setName("StarterPack");
	}

	//97.87% match
	//le EPIC TROLL
	void StarterPackService::render2d(Adorn* adorn)
	{
		TopMenuBar::render2d(adorn);

		if (findFirstChildOfType<BackpackItem>())
		{
			G3D::Rect2D viewPort = adorn->getViewport();
			int fontSize = GuiRoot::normalizedFontSize(12);
			G3D::Vector2 pos2d = viewPort.x0y1() + G3D::Vector2(4.0f, -4.0f);

			adorn->drawFont2D(
				"StarterPack - these items will be given to each new player", 
				pos2d, 
				fontSize, 
				G3D::Color3::white(), 
				G3D::Color4::clear(), 
				Adorn::XALIGN_LEFT, 
				Adorn::YALIGN_BOTTOM,
				Adorn::PROPORTIONAL_SPACING
			);
		}
	}

	LegacyHopperService::LegacyHopperService()
	{
		setName("Hopper");
	}

	LegacyHopperService::~LegacyHopperService()
	{
		RBXASSERT(numChildren() == 0);
	}

	void LegacyHopperService::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Instance::onServiceProvider(oldProvider, newProvider);

		if (newProvider)
		{
			StarterPackService* starterPack = newProvider->find<StarterPackService>();
			RBXASSERT(starterPack);

			while (numChildren() > 0)
			{
				getChild(0)->setParent(starterPack);
			}

			setParent(NULL);
		}
	}
}
