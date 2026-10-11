#include "gui/HealthHud.h"
#include "humanoid/Humanoid.h"
#include "v8datamodel/Visit.h"
#include "util/Color.h"

namespace RBX
{
	HealthHud::HealthHud()
	{
		setGuiSize(G3D::Vector2(10.0f, 100.0f));
	}

	void HealthHud::render2d(Adorn* adorn)
	{
		setGuiSize(G3D::Vector2(0.1f, 1.0f) * (adorn->getViewport().height() * 0.125f));

		int textSize = (int)(getGuiSize().y * 0.18f);

		if (!visit)
			visit = RBX::shared_from(ServiceProvider::find<Visit>(this));

		if (visit)
		{
			if (Humanoid* humanoid = Humanoid::getLocalHumanoidFromContext(this))
			{
				float relativeHealth = humanoid->relativeHealth();

				Rect rect = getMyRect();

				Rect greenBarRect = rect;
				Rect redBarRect = rect;

				greenBarRect.low.y = rect.high.y - rect.size().y * humanoid->relativeHealth();
				redBarRect.high.y = greenBarRect.low.y;

				adorn->rect2d(greenBarRect.toRect2D(), Color::lightGreen());
				adorn->rect2d(redBarRect.toRect2D(), G3D::Color3::red());

				adorn->drawFont2D(
					"Health",
					rect.positionPoint(Rect::CENTER, Rect::BOTTOM),
					textSize,
					G3D::Color3::blue(),
					G3D::Color4::clear(),
					Adorn::XALIGN_CENTER,
					Adorn::YALIGN_TOP,
					Adorn::PROPORTIONAL_SPACING
				);
			}
		}
	}
}
