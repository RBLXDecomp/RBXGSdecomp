#include "gui/Gui.h"
#include "v8tree/Verb.h"

namespace RBX
{
	class DataModel;
	class Workspace;
	class ChatOption;
	class UnifiedWidget;

	class GuiBuilder
	{
	private:
		DataModel* dataModel;
		Workspace* workspace;

	private:
		boost::shared_ptr<TopMenuBar> buildMenu();
		boost::shared_ptr<TopMenuBar> buildRightPalette(Adorn* adorn);
		boost::shared_ptr<TopMenuBar> buildRightControlPalette(Adorn*, GuiRoot*);
		boost::shared_ptr<TopMenuBar> buildScoreHud();
		boost::shared_ptr<TopMenuBar> buildScoreHud(int);
		boost::shared_ptr<TopMenuBar> buildControlHud(Adorn* adorn, int yOffset);
		boost::shared_ptr<TopMenuBar> buildMessageHud(int);
		boost::shared_ptr<TopMenuBar> buildChatHud();
		void buildChatMenu(ChatOption* parentOption, std::string parentCode, boost::shared_ptr<UnifiedWidget> parentGui);
		boost::shared_ptr<TopMenuBar> buildChatMenu(Adorn* adorn);
		boost::shared_ptr<TopMenuBar> buildHealthHud();
		boost::shared_ptr<TopMenuBar> buildStatsHud1();
		boost::shared_ptr<TopMenuBar> buildStatsHud2();
		void buildRightPaletteItems(Adorn* adorn, boost::shared_ptr<TopMenuBar> panel);
		TopMenuBar* buildColorPanel(Adorn*, GuiRoot*);
		Verb* getVerb(const std::string& name);
	public:
		void buildGui(Adorn* adorn, DataModel* dataModel, Workspace* workspace);
	};
}
