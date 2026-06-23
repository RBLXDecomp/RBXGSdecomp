#include "v8datamodel/Hopper.h"
#include "v8datamodel/PlayerHopper.h"
#include "v8datamodel/Workspace.h"

namespace RBX
{
	Reflection::PropDescriptor<PlayerHopperItem, TextureId> desc_TextureId("TextureId", "Data", &PlayerHopperItem::getTextureId, &PlayerHopperItem::setTextureId, Reflection::PropertyDescriptor::STANDARD);

	Reflection::PropDescriptor<HopperBin, std::string> desc_Command("Command", "Data", NULL, &HopperBin::setCommand, Reflection::PropertyDescriptor::STANDARD);
	Reflection::PropDescriptor<HopperBin, std::string> desc_TextureName("TextureName", "Data", NULL, &HopperBin::setTextureName, Reflection::PropertyDescriptor::STANDARD);
	Reflection::EnumPropDescriptor<HopperBin, HopperBin::BinType> desc_BinType("BinType", "Data", &HopperBin::getBinType, &HopperBin::setBinType, Reflection::PropertyDescriptor::STANDARD);

	Reflection::SignalDesc<HopperBin, void(void)> desc_Deselected("Deselected");
	Reflection::SignalDesc<HopperBin, void(boost::shared_ptr<Instance>)> desc_Selected("Selected", "mouse");

	Hopper::Hopper()
	{
		yLocation = Rect::BOTTOM;
	}

	bool Hopper::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const PlayerHopperItem>(instance) != NULL;
	}

	bool Hopper::askSetParent(const Instance* instance) const
	{
		return true;
	}

	bool PlayerHopperItem::askSetParent(const Instance* instance) const
	{
		return true;
	}

	bool PlayerHopperItem::askAddChild(const Instance* instance) const
	{
		return true;
	}

	const TextureId PlayerHopperItem::getTextureId() const
	{
		return textureId;
	}

	bool PlayerHopperItem::isEnabled()
	{
		return getParent() && fastDynamicCast<PlayerHopper>(getParent()) != NULL;
	}

	void PlayerHopperItem::setTextureId(const TextureId& value)
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

	G3D::Vector2 PlayerHopperItem::getSize() const
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
				setTextureName(textureName);
			}
		}
	}

	void HopperBin::setTextureName(const std::string& value)
	{
		TextureId textureId = ContentId::fromAssets("Textures\\" + value + ".png");
		setTextureId(textureId);
	}

	void HopperBin::setCommand(const std::string& text)
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

	HopperService::HopperService()
	{
		setName("Hopper");
	}

}
