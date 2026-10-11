#include "v8datamodel/GuiBuilder.h"
#include "v8datamodel/DataModel.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/SafeChat.h"
#include "gui/Gui.h"
#include "gui/MenuItem.h"
#include "gui/Button.h"
#include "gui/ArrowPanel.h"
#include "gui/HealthHud.h"
#include "gui/ScoreHud.h"
#include "gui/ChatOutput.h"
#include "gui/ChatWidget.h"
#include "gui/EquationDisplay.h"

namespace RBX
{
	Verb* GuiBuilder::getVerb(const std::string& name)
	{
		RBXASSERT(workspace);

		Verb* verb = workspace->getVerb(name);
		RBXASSERT(verb);

		return verb;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildMenu()
	{
		Layout panelLayout;
		panelLayout.xLocation = Rect::LEFT;
		panelLayout.yLocation = Rect::TOP;
		panelLayout.layoutStyle = Layout::HORIZONTAL;
		panelLayout.backdropColor = GuiItem::translucentBackdrop();
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("MainMenu");

		panel->addGuiItem(Creatable<Instance>::create<MenuItem>(getVerb("TogglePlayMode"), "Tools"));
		panel->addGuiItem(Creatable<Instance>::create<MenuItem>(getVerb("ToggleToolbox"), "Insert"));
		panel->addGuiItem(Creatable<Instance>::create<MenuItem>(getVerb("ToggleFullScreen"), "Fullscreen"));
		panel->addGuiItem(Creatable<Instance>::create<MenuItem>(getVerb("Help"), "Help..."));
		panel->addGuiItem(Creatable<Instance>::create<MenuItem>(getVerb("Exit"), "   Exit"));

		for (size_t i = 0; i < panel->numChildren(); ++i)
		{
			if (GuiItem* guiItem = panel->getGuiItem((int)i))
			{
				guiItem->setGuiSize(G3D::Vector2(100.0f, 20.0f));
			}
		}

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildRightPalette(Adorn* adorn)
	{
		Layout panelLayout;
		panelLayout.xLocation = Rect::RIGHT;
		panelLayout.yLocation = Rect::BOTTOM;
		panelLayout.layoutStyle = Layout::VERTICAL;
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("RightPalette");

		buildRightPaletteItems(adorn, panel);

		return panel;
	}

	void GuiBuilder::buildRightPaletteItems(Adorn* adorn, boost::shared_ptr<TopMenuBar> panel)
	{
		panel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("FirstPersonIndicator"), "FirstPersonIndicator"));

		panel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("ReportAbuse"), "ReportAbuse"));

		boost::shared_ptr<TopMenuBar> cameraLeftPanel = Creatable<Instance>::create<TopMenuBar>("CameraLeft", Layout::VERTICAL);
		cameraLeftPanel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("CameraTiltUp"), "Tilt Up"));
		cameraLeftPanel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("CameraTiltDown"), "Tilt Down"));

		boost::shared_ptr<TopMenuBar> cameraRightPanel = Creatable<Instance>::create<TopMenuBar>("CameraRight", Layout::VERTICAL);
		cameraRightPanel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("CameraZoomIn"), "Zoom In"));
		cameraRightPanel->addGuiItem(Creatable<Instance>::create<ImageButton>(adorn, getVerb("CameraZoomOut"), "Zoom Out"));

		boost::shared_ptr<TopMenuBar> cameraPanel = Creatable<Instance>::create<TopMenuBar>("CameraPanel", Layout::HORIZONTAL);
		cameraPanel->addGuiItem(cameraLeftPanel);
		cameraPanel->addGuiItem(cameraRightPanel);

		panel->addGuiItem(cameraPanel);
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildControlHud(Adorn* adorn, int yOffset)
	{
		Layout panelLayout;
		panelLayout.offset.y = yOffset;
		panelLayout.xLocation = Rect::RIGHT;
		panelLayout.yLocation = Rect::TOP;
		panelLayout.layoutStyle = Layout::VERTICAL;
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("MainMenu");

		panel->addGuiItem(Creatable<Instance>::create<ArrowPanel>(adorn, dataModel, Controller::PRIMARY_CONTROLLER));
		panel->addGuiItem(Creatable<Instance>::create<MenuSpacer>(5.0f, 5.0f));
		panel->addGuiItem(Creatable<Instance>::create<ArrowPanel>(adorn, dataModel, Controller::SECONDARY_CONTROLLER));

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildHealthHud()
	{
		Layout panelLayout;
		panelLayout.offset.x = 60;
		panelLayout.offset.y = 200;
		panelLayout.xLocation = Rect::RIGHT;
		panelLayout.yLocation = Rect::TOP;
		panelLayout.layoutStyle = Layout::VERTICAL;
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("HealthHud");

		panel->addGuiItem(Creatable<Instance>::create<HealthHud>());

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildScoreHud()
	{
		Layout panelLayout;
		panelLayout.offset.x = 10;
		panelLayout.offset.y = 10;
		panelLayout.xLocation = Rect::RIGHT;
		panelLayout.yLocation = Rect::TOP;
		panelLayout.layoutStyle = Layout::VERTICAL;
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("ScoreHud");

		panel->addGuiItem(Creatable<Instance>::create<ScoreHud>(dataModel));

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildChatHud()
	{
		Layout panelLayout;
		panelLayout.offset.x = 25;
		panelLayout.offset.y = 100;
		panelLayout.xLocation = Rect::LEFT;
		panelLayout.yLocation = Rect::TOP;
		panelLayout.layoutStyle = Layout::VERTICAL;
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("ChatHud");

		panel->addGuiItem(Creatable<Instance>::create<ChatOutput>());

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildChatMenu(Adorn* adorn)
	{
		boost::shared_ptr<PercentPanel> panel = Creatable<Instance>::create<PercentPanel>(Rect::LEFT, Rect::BOTTOM, G3D::Vector2(2.0f, 26.0f));
		panel->setName("ChatMenuPanel");

		boost::shared_ptr<ChatButton> chatButton = Creatable<Instance>::create<ChatButton>(adorn, "Chat");
		panel->addGuiItem(chatButton);

		ChatOption* chatRoot = SafeChat::singleton().getChatRoot();
		std::string code = "";
		buildChatMenu(chatRoot, code, chatButton);

		return panel;
	}

	void GuiBuilder::buildChatMenu(ChatOption* parentOption, std::string parentCode, boost::shared_ptr<UnifiedWidget> parentGui)
	{
		for (size_t i = 0; i < parentOption->children.size(); ++i)
		{
			ChatOption* option = parentOption->children[i];
			std::string code = parentCode + " " + boost::lexical_cast<std::string>(i);

			boost::shared_ptr<ChatWidget> button = Creatable<Instance>::create<ChatWidget>(option->text, code);
			parentGui->addGuiItem(button);

			buildChatMenu(option, code, button);
		}
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildStatsHud1()
	{
		Layout panelLayout;
		panelLayout.offset.x = 50;
		panelLayout.offset.y = 40;
		panelLayout.xLocation = Rect::LEFT;
		panelLayout.yLocation = Rect::BOTTOM;
		panelLayout.layoutStyle = Layout::VERTICAL;
		panelLayout.backdropColor = GuiItem::translucentBackdrop();
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("StatsHud1");

		panel->addGuiItem(Creatable<Instance>::create<TextDisplay>("", "-- Performance --"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("FPS           : ", "FPS"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Sim           : ", "simDC"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Gfx           : ", "GfxTime"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Mode          : ", "graphicsMode"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Video Memory  : ", "videoMemory"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Culling       : ", "culling"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Culling %     : ", "cullingPercent"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Anti-Aliasing : ", "antiAliasing"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Bevels        : ", "bevels"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("FrameId       : ", "drawId"));

		for (size_t i = 0; i < panel->numChildren(); ++i)
		{
			boost::shared_ptr<TextDisplay> item = shared_from_dynamic_cast<TextDisplay>(panel->getChild(i));
			RBXASSERT(item);

			item->setFontSize(14);
			item->setFontColor(G3D::Color3::purple());
			item->setBorderColor(G3D::Color4::clear());
			item->setGuiSize(G3D::Vector2(310.0f, 22.0f));
		}

		panel->setVisible(false);

		return panel;
	}

	boost::shared_ptr<TopMenuBar> GuiBuilder::buildStatsHud2()
	{
		Layout panelLayout;
		panelLayout.offset.x = 50;
		panelLayout.offset.y = 40;
		panelLayout.xLocation = Rect::RIGHT;
		panelLayout.yLocation = Rect::BOTTOM;
		panelLayout.layoutStyle = Layout::VERTICAL;
		panelLayout.backdropColor = GuiItem::translucentBackdrop();
		boost::shared_ptr<RelativePanel> panel = Creatable<Instance>::create<RelativePanel>(panelLayout);
		panel->setName("StatsHud2");

		panel->addGuiItem(Creatable<Instance>::create<TextDisplay>("", "----- World ----"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Primitives    : ", "numPrimitives"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Joints        : ", "numJoints"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Contacts      : ", "numContacts"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Tree          : ", "maxTreeDepth"));

		panel->addGuiItem(Creatable<Instance>::create<TextDisplay>("", "----- Contacts ----"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("CtctStageCtcts: ", "numContactsInCollisionStage"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("SteppingCtcts : ", "numSteppingContacts"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("TouchingCtcts : ", "numTouchingContacts"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Pair Hit Ratio: ", "contactPairHitRatio"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("# link(p)     : ", "numLinkCalls"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("H Nodes Out   : ", "numHashNodes"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Max Bucket    : ", "maxBucketSize"));

		panel->addGuiItem(Creatable<Instance>::create<TextDisplay>("", "----- Kernel ----"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Bodies        : ", "numBodies"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Constraints   : ", "numConstraints"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Points        : ", "numPoints"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Energy B      : ", "energyBody"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Energy C      : ", "energyConnector"));
		panel->addGuiItem(Creatable<Instance>::create<EquationDisplay>("Energy T      : ", "energyTotal"));

		for (size_t i = 0; i < panel->numChildren(); ++i)
		{
			boost::shared_ptr<TextDisplay> item = shared_from_dynamic_cast<TextDisplay>(panel->getChild(i));
			RBXASSERT(item);

			item->setFontSize(14);
			item->setFontColor(G3D::Color3::purple());
			item->setBorderColor(G3D::Color4::clear());
			item->setGuiSize(G3D::Vector2(310.0f, 22.0f));
		}

		panel->setVisible(false);

		return panel;
	}

	void GuiBuilder::buildGui(Adorn* adorn, DataModel* _dataModel, Workspace* _workspace)
	{
		dataModel = _dataModel;
		workspace = _workspace;

		dataModel->getGuiRoot()->addGuiItem(buildStatsHud1());
		dataModel->getGuiRoot()->addGuiItem(buildStatsHud2());
		dataModel->getGuiRoot()->addGuiItem(buildChatHud());
		dataModel->getGuiRoot()->addGuiItem(buildChatMenu(adorn));
		dataModel->getGuiRoot()->addGuiItem(buildScoreHud());
		dataModel->getGuiRoot()->addGuiItem(buildHealthHud());
		dataModel->getGuiRoot()->addGuiItem(buildMenu());
		dataModel->getGuiRoot()->addGuiItem(buildRightPalette(adorn));
		dataModel->getGuiRoot()->addGuiItem(buildControlHud(adorn, 20));
	}
}
