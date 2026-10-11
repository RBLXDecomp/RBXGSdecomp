#pragma once
#include "gui/Widget.h"

namespace RBX
{
	class DataModel;

	class CurrentColor : public Widget
	{
	private:
		DataModel* dataModel;

	protected:
		virtual void render2d(Adorn*);
	public:
		CurrentColor(DataModel*);
	};

	class ColorButton : public Widget
	{
	private:
		DataModel* dataModel;
		G3D::Color4 color;

	protected:
		virtual void render2d(Adorn*);
	public:
		ColorButton(DataModel*, const G3D::Color4&);
		virtual void onClick(GuiEvent&);
	};

	class ColorPicker : public TopMenuBar
	{
	private:
		void addFourItems(DataModel*, boost::shared_ptr<TopMenuBar>, const G3D::Color3&);
	public:
		ColorPicker(DataModel*);
	};
}
