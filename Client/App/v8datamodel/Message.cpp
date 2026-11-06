#include "v8datamodel/Message.h"
#include "util/Rect.h"
#include "Network/Player.h"

namespace RBX
{
	char* sMessage = "Message";

	static Reflection::PropDescriptor<Message, std::string> desc_Text("Text", "Appearance", &Message::getText, &Message::setText, Reflection::PropertyDescriptor::STANDARD);

	Message::Message() 
		: Base("Message")
	{
	}

	void Message::setText(const std::string& value)
	{
		if (text != value)
		{
			text = value;
			raisePropertyChanged(desc_Text);
		}
	}

	void Message::render2d(Adorn* adorn)
	{
		if (text.size() > 0)
		{
			if (!dynamic_cast<Network::Player*>(getParent()))
			{
				renderFullScreen(adorn);
			}
			else
			{
				renderPersonalMsg(adorn);
			}
		}
	}

	void Message::renderFullScreen(Adorn* adorn)
	{
		G3D::Rect2D rect = adorn->getViewport();
		Rect translucentArea = rect;
		G3D::Vector2 textPos = translucentArea.center();

		adorn->rect2d(translucentArea.toRect2D(), G3D::Color4(0.5f, 0.5f, 0.5f, 0.5f));
		adorn->drawFont2D(text, textPos, 14.0, G3D::Color3::white(), G3D::Color3::black(), Adorn::XALIGN_CENTER, Adorn::YALIGN_CENTER, Adorn::PROPORTIONAL_SPACING);
	}

	void Message::renderPersonalMsg(Adorn* adorn)
	{
		G3D::Rect2D rect = adorn->getViewport();
		Rect translucentArea(rect.x0() + 30.0f, rect.y0() + 30.0f, rect.x1() * 0.25f + 30.0f, rect.y1() * 0.25f + 30.0f);
		G3D::Vector2 textPos = translucentArea.center();

		adorn->rect2d(translucentArea.toRect2D(), G3D::Color4(0.5f, 0.5f, 0.5f, 0.5f));
		adorn->drawFont2D(text, textPos, 14.0, G3D::Color3::white(), G3D::Color3::black(), Adorn::XALIGN_CENTER, Adorn::YALIGN_CENTER, Adorn::PROPORTIONAL_SPACING);
	}

	char* sHint = "Hint";

	void Hint::render2d(Adorn* adorn)
	{
		G3D::Rect2D rect = adorn->getViewport();
		Rect blackArea(rect.x0(), rect.y1() - 20.0f, rect.x1(), rect.y1());
		G3D::Vector2 textPos = blackArea.center();

		adorn->rect2d(blackArea.toRect2D(), G3D::Color3::black());
		adorn->drawFont2D(text, textPos, 14.0, G3D::Color3::white(), G3D::Color4::clear(), Adorn::XALIGN_CENTER, Adorn::YALIGN_CENTER, Adorn::PROPORTIONAL_SPACING);
	}
}
