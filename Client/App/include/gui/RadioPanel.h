#pragma once
#include "gui/Gui.h"
#include "gui/Button.h"
#include "v8tree/Verb.h"

namespace RBX
{
	class RadioPanelVerb;

	class RadioPanel : public VerbContainer, public TopMenuBar
	{
	private:
		std::vector<RadioPanelVerb*> verbs;
		std::vector<TopMenuBar*> panels;
		TopMenuBar* visiblePanel;

	public:
		RadioPanel(const std::string&, Layout::Style, bool);
		virtual ~RadioPanel();
		void addRadioItem(Adorn*, const std::string&, TopMenuBar*);
		void addRadioItemText(Adorn*, const std::string&, const std::string&, TopMenuBar*);
		TopMenuBar* getVisiblePanel();
		void setVisiblePanel(TopMenuBar*);
	};

	class RadioPanelVerb : public Verb
	{
	private:
		RadioPanel* radioPanel;
		TopMenuBar* panel;

	public:
		RadioPanelVerb(RadioPanel*, const std::string&, TopMenuBar*);
		virtual bool isEnabled() const;
		virtual void doIt(IDataState*);
	};

	class RadioTextButton : public TextButton
	{
	public:
		RadioTextButton(Adorn*, Verb*, const std::string&);
		virtual void render2d(Adorn*);
	};
}
