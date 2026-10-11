#pragma once
#include "gui/Gui.h"

namespace RBX
{
	class Visit;

	class HealthHud : public GuiItem
	{
	private:
		boost::shared_ptr<Visit> visit;

	private:
		virtual void render2d(Adorn* adorn);
	public:
		HealthHud();
	};
}
