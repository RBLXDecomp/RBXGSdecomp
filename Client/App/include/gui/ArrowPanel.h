#pragma once
#include "gui/Button.h"
#include "v8world/Controller.h"

namespace RBX
{
	class DataModel;

	class ArrowButton : public ImageKeyButton
	{
	private:
		DataModel* dataModel;
		Controller::ControllerType controller;

	public:
		ArrowButton(Adorn* adorn, const std::string& name, SDLKey key, Controller::ControllerType controller, DataModel* dataModel);
		virtual bool isEnabled();
	};

	class ArrowPanel : public TopMenuBar
	{
	protected:
		DataModel* dataModel;
		Controller::ControllerType controller;

	protected:
		void init(
			Adorn* adorn,
			const std::string& iconName,
			const std::string& key1Name,
			const std::string& key2Name,
			const std::string& key3Name,
			const std::string& key4Name,
			SDLKey key1,
			SDLKey key2,
			SDLKey key3,
			SDLKey key4);
	public:
		ArrowPanel(Adorn* adorn, DataModel* dataModel, Controller::ControllerType controller);
		virtual bool isVisible() const;
		virtual G3D::Vector2 getSize() const;
	};
}
