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
			void doLock(const DataModel* dataModel);
		public:
			Lock(const DataModel* dataModel);
			Lock(boost::shared_ptr<const DataModel> dataModel);
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
		std::string httpGet(std::string url, bool synchronous);
		std::string httpPost(std::string url, std::string data, bool synchronous);
		void close();
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> get(ContentId);
		void raiseClose();
		int numChatOptions();
		void buildGui(Adorn*);
		void processGui(const UIEvent&);
		GuiRoot* getGuiRoot() const;
		GuiRoot* getGuiHooks() const;

		virtual void setDirty(bool dirty)
		{
			this->dirty = dirty;
		}

		virtual bool isDirty() const
		{
			return dirty;
		}

		virtual bool isStackTooBig() const;
		Workspace* getWorkspace() const;
		float step(float);
		float getSimTime() const;
		TimeState* getTimeState();
		virtual std::string evaluate(const std::string&) const;
		void renderPass2d(Adorn*, IMetric*);
		void renderPass3dAdorn(Adorn*);

		void setUiMessage(std::string message)
		{
			uiMessage = message;
		}

		void clearUiMessage()
		{
			uiMessage = "";
		}

		void setUiMessageBrickCount()
		{
			uiMessage = "[[[progress]]]";
		}

		bool canDelaySwapBuffer() const;
	protected:
		virtual bool askAddChild(const Instance* instance) const;
		virtual void onChildAdded(Instance* child);

		virtual void onChildChanged(Instance* instance, const PropertyChanged& event)
		{
			Instance::onChildChanged(instance, event);
			setDirty(true);
		}

		virtual void onDescendentAdded(Instance* instance)
		{
			ServiceProvider::onDescendentAdded(instance);
			setDirty(true);
		}

		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
		{
			ServiceProvider::onDescendentRemoving(instance);
			setDirty(true);
		}

		virtual void onEvent(const RunService* source, RunTransition event);

	public:
		static boost::shared_ptr<boost::recursive_mutex> getMutex(Instance*);
		static boost::shared_ptr<DataModel> createDataModel();
		static void closeDataModel(boost::shared_ptr<DataModel>, bool);
	private:
		static std::string doHttpGet(std::string url);
		static std::string doHttpPost(std::string url, std::string data);
	};
}
