#pragma once
#include "util/RunStateOwner.h"
#include "util/Evaluator.h"
#include "v8tree/Verb.h"
#include "gui/GuiEvent.h"

namespace RBX
{
	class LocalBackpack;
	class Workspace;
	class StarterPackService;
	class GuiRoot;
	class TimeState;
	class IMetric;

	extern const char* sDataModel;

	class DataModel : public DescribedNonCreatable<DataModel, ServiceProvider, &sDataModel>,
					  public VerbContainer,
					  public IDataState,
					  public Evaluator,
					  public Listener<RunService, RunTransition>
	{
	public:
		class Lock
		{
		private:
			boost::shared_ptr<boost::recursive_mutex> mutex;
			boost::recursive_mutex::scoped_lock* lock;

		private:
			void doLock(const DataModel*);
		public:
			Lock(const DataModel*);
			Lock(boost::shared_ptr<const DataModel*>);
			~Lock();
		};

	private:
		boost::shared_ptr<LocalBackpack> localBackpack;
		boost::shared_ptr<Workspace> workspace;
		boost::shared_ptr<RunService> runService;
		boost::shared_ptr<StarterPackService> starterPackService;
		boost::shared_ptr<GuiRoot> guiRoot;
		boost::shared_ptr<GuiRoot> guiHooks;
		GuiTarget* guiTarget;
		bool forceArrowCursor;
		std::string uiMessage;
		int drawId;
		IMetric* tempMetric;
		bool dirty;
		boost::shared_ptr<boost::recursive_mutex> mutex;
		boost::scoped_ptr<TimeState> timeState;

	private:
		GuiResponse processAccelerators(const GuiEvent&);
		void processEvent(const UIEvent&);
		void initializeContents();
	public:
		void clearContents();
		boost::shared_ptr<boost::recursive_mutex> getMutex() const;
		DataModel();
		virtual ~DataModel();
		void loadContent(ContentId);
		void save(ContentId);
		std::string httpGet(std::string, bool);
		std::string httpPost(std::string, std::string, bool);
		void close();
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> get(ContentId);
		void raiseClose();
		int numChatOptions();
		void buildGui(Adorn*);
		void processGui(const UIEvent&);
		GuiRoot* getGuiRoot() const;
		GuiRoot* getGuiHooks() const;
		virtual void setDirty(bool);
		virtual bool isDirty() const;
		virtual bool isStackTooBig() const;
		Workspace* getWorkspace() const;
		float step(float);
		float getSimTime() const;
		TimeState* getTimeState();
		virtual std::string evalute(const std::string&) const;
		void renderPass2d(Adorn*, IMetric*);
		void renderPass3dAdorn(Adorn*);
		void setUiMessage(std::string);
		void clearUiMessage();
		void setUiMessageBrickCount();
		bool canDelaySwapBuffer() const;
	protected:
		virtual bool askAddChild(const Instance*) const;
		virtual void onChildAdded(Instance*);
		virtual void onChildChanged(Instance*, const PropertyChanged&);
		virtual void onDescendentAdded(Instance*);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);
		virtual void onEvent(const RunService*, RunTransition);

	public:
		static boost::shared_ptr<boost::recursive_mutex> getMutex(Instance*);
		static boost::shared_ptr<DataModel> createDataModel();
		static void closeDataModel(boost::shared_ptr<DataModel>, bool);
		static std::string doHttpGet(std::string);
		static std::string doHttpPost(std::string, std::string);
	};
}
