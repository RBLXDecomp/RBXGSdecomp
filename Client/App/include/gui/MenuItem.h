#pragma once
#include "gui/Widget.h"

namespace RBX
{
	class MenuItem : public VerbWidget
	{
	private:
		std::string accelerator;

	protected:
		virtual bool canLoseFocus()
		{
			return true;
		}
	public:
		MenuItem(Verb*, const std::string&, std::string);
		MenuItem(Verb* verb, const std::string& name);
		virtual void render2d(Adorn* adorn);
	};
}
