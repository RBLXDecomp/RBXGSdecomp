#include "gui/MenuItem.h"

namespace RBX
{
	MenuItem::MenuItem(Verb* verb, const std::string& name)
		: VerbWidget(verb)
	{
		setName(name);
		setGuiSize(G3D::Vector2(200.0f, 24.0f));
	}

	void MenuItem::render2d(Adorn* adorn)
	{
		if (widgetState == HOVER || widgetState == DOWN_OVER)
			adorn->rect2d(getMyRect2D(), menuSelect());

		bool enabled = isEnabled();
		label2d(
			adorn,
			getVerb() && getVerb()->isChecked() ? "x " + getTitle() : getTitle(),
			isEnabled() ? menuEnabledFill() : menuDisabledFill(),
			G3D::Color4::clear(),
			Adorn::XALIGN_LEFT
		);

		if (accelerator.size() > 0)
		{
			label2d(
				adorn,
				accelerator,
				isEnabled() ? menuEnabledFill() : menuDisabledFill(),
				G3D::Color4::clear(),
				Adorn::XALIGN_RIGHT
			);
		}
	}
}
