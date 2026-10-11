#include "gui/ArrowPanel.h"
#include "v8datamodel/DataModel.h"
#include "v8datamodel/Workspace.h"
#include "util/RunStateOwner.h"

namespace RBX
{
	ArrowButton::ArrowButton(Adorn* adorn, const std::string& name, SDLKey key, Controller::ControllerType controller, DataModel* dataModel)
		: ImageKeyButton(adorn, name, key),
		  dataModel(dataModel),
		  controller(controller)
	{
	}

	bool ArrowButton::isEnabled()
	{
		return ServiceProvider::create<RunService>(dataModel)->isRunState();
	}

	ArrowPanel::ArrowPanel(Adorn* adorn, DataModel* dataModel, Controller::ControllerType controller)
		: TopMenuBar("ArrowPanel", Layout::HORIZONTAL),
		  controller(controller),
		  dataModel(dataModel)
	{
		switch (controller)
		{
		case Controller::PLAYER_CONTROLLER:
			init(adorn, "Character", "W_key", "S_key", "A_key", "D_key", SDLK_w, SDLK_s, SDLK_a, SDLK_d);
			break;

		case Controller::PRIMARY_CONTROLLER:
			init(adorn, "Controller1", "U_key", "J_key", "H_key", "K_key", SDLK_u, SDLK_j, SDLK_h, SDLK_k);
			break;

		case Controller::SECONDARY_CONTROLLER:
			init(adorn, "Controller2", "W_key", "S_key", "A_key", "D_key", SDLK_w, SDLK_s, SDLK_a, SDLK_d);
			break;

		default:
			RBXASSERT(false);
		}
	}

	void ArrowPanel::init(
		Adorn* adorn,
		const std::string& iconName,
		const std::string& key1Name,
		const std::string& key2Name,
		const std::string& key3Name,
		const std::string& key4Name,
		SDLKey key1,
		SDLKey key2,
		SDLKey key3,
		SDLKey key4)
	{
		boost::shared_ptr<TopMenuBar> leftPanel = Creatable<Instance>::create<TopMenuBar>("LeftPanel", Layout::VERTICAL);
		leftPanel->addGuiItem(Creatable<Instance>::create<ImageWidget>(adorn, iconName));
		leftPanel->addGuiItem(Creatable<Instance>::create<ArrowButton>(adorn, key3Name, key3, controller, dataModel));

		boost::shared_ptr<TopMenuBar> forwardBackPanel = Creatable<Instance>::create<TopMenuBar>("ForwardBackPanel", Layout::VERTICAL);
		forwardBackPanel->addGuiItem(Creatable<Instance>::create<ArrowButton>(adorn, key1Name, key1, controller, dataModel));
		forwardBackPanel->addGuiItem(Creatable<Instance>::create<ArrowButton>(adorn, key2Name, key2, controller, dataModel));

		boost::shared_ptr<TopMenuBar> middlePanel = Creatable<Instance>::create<TopMenuBar>(key4Name, Layout::VERTICAL);
		middlePanel->addGuiItem(Creatable<Instance>::create<MenuSpacer>(32.0f, 32.0f));
		middlePanel->addGuiItem(Creatable<Instance>::create<ArrowButton>(adorn, key4Name, key4, controller, dataModel));

		addGuiItem(leftPanel);
		addGuiItem(forwardBackPanel);
		addGuiItem(middlePanel);
	}

	bool ArrowPanel::isVisible() const
	{
		return dataModel->getWorkspace()->getControllersUsed().hasController(controller);
	}

	G3D::Vector2 ArrowPanel::getSize() const
	{
		if (isVisible())
			return TopMenuBar::getSize();

		return G3D::Vector2::zero();
	}
}
