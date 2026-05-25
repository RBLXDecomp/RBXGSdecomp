#include "v8datamodel/Message.h"
#include "util/Rect.h"
#include "Network/Player.h"

namespace RBX
{
	Reflection::PropDescriptor<Message, std::string> desc_Text("Text", "Appearance", &Message::getText, &Message::setText, Reflection::PropertyDescriptor::STANDARD);

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
			if (!fastDynamicCast<Network::Player>(getParent()))
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
		G3D::Vector2 textPos = rect.center();

		adorn->rect2d(translucentArea.toRect2D(), G3D::Color4(0.5f, 0.5f, 0.5f, 0.5f));
		adorn->drawFont2D(text, textPos, 14.0, G3D::Color3::white(), G3D::Color3::black(), Adorn::XALIGN_CENTER, Adorn::YALIGN_CENTER, Adorn::PROPORTIONAL_SPACING);
	}
}
