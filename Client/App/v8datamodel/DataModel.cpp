#include "v8datamodel/DataModel.h"
#include "v8datamodel/TimeState.h"
#include "v8datamodel/Workspace.h"
#include "gui/GUI.h"
#include "util/standardout.h"
#include "util/Http.h"

namespace RBX
{

#pragma warning (push)
#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list

	//99% match
	DataModel::DataModel()
		: VerbContainer(NULL),
		  workspace(Creatable::create<Workspace>(this)),
		  guiRoot(Creatable::create<GuiRoot>()),
		  guiHooks(Creatable::create<GuiRoot>()),
		  timeState(new TimeState),
		  mutex(new boost::recursive_mutex),
		  guiTarget(NULL),
		  forceArrowCursor(true),
		  dirty(false),
		  drawId(0)
	{
		setName("Level");
	}

#pragma warning (pop)

	DataModel::~DataModel()
	{
		if (runService)
			runService->Notifier<RunService, RunTransition>::removeListener(this);

		StandardOut::singleton()->print(MESSAGE_INFO, "~DataModel");
	}

	bool DataModel::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const Service>(instance) != NULL;
	}

	boost::shared_ptr<DataModel> DataModel::createDataModel()
	{
		ContentProvider::singleton().clearContentCache();
		
		boost::shared_ptr<DataModel> dataModel = Creatable::create<DataModel>();

		{
			Lock lock(dataModel);
			dataModel->initializeContents();
		}

		return dataModel;
	}
	
	void DataModel::onChildAdded(Instance* child)
	{
		ServiceProvider::onChildAdded(child);

		if (child->getClassName() == "NetworkClient")
		{
			Verb* lockPlayMode = getVerb("LockPlayMode");
			if (lockPlayMode)
				lockPlayMode->doIt(NULL);

			workspace->setNullMouseCommand();
		}
	}

	void DataModel::onEvent(const RunService* source, RunTransition event)
	{
		switch (event.newState)
		{
		case RS_NORMAL:
			if (event.oldState == RS_RUNNING)
				workspace->reset();

			timeState->clear();
			break;
		case RS_RUNNING:
			workspace->start();
			break;
		case RS_PAUSED:
			workspace->stop();
			break;
		}
	}

	std::string DataModel::doHttpGet(std::string url)
	{
		std::string result;
		Http(url.c_str()).get(result);

		return result;
	}

	std::string DataModel::doHttpPost(std::string url, std::string data)
	{
		std::string result;
		Http(url).post(std::istringstream(data), true, result);

		return result;
	}

	void DataModel::close()
	{
		Verb* exitVerb = getVerb("Exit");
		if (!exitVerb)
			throw std::runtime_error("Couldn\'t find Exit Verb");

		exitVerb->doIt(NULL);
	}

	std::string DataModel::httpGet(std::string url, bool synchronous)
	{
		if (synchronous)
		{
			return doHttpGet(url);
		}
		else
		{
			if (url.size() > 0)
			{
				boost::function0<void> f = boost::bind(&DataModel::doHttpGet, url);
				boost::function0<void> g = boost::bind(&StandardOut::print_exception, f, MESSAGE_ERROR, false);

				boost::thread(background_function(g, "rbx_httpGet"));
			}

			return "";
		}
	}

	std::string DataModel::httpPost(std::string url, std::string data, bool synchronous)
	{
		if (synchronous)
		{
			return doHttpPost(url, data);
		}
		else
		{
			boost::function0<void> f = boost::bind(&DataModel::doHttpPost, url, data);
			boost::function0<void> g = boost::bind(&StandardOut::print_exception, f, MESSAGE_ERROR, false);

			boost::thread(background_function(g, "rbx_httpPost"));

			return "";
		}
	}

	void DataModel::Lock::doLock(const DataModel* dataModel)
	{
		if (dataModel)
		{
			mutex = dataModel->mutex;
			lock = new boost::recursive_mutex::scoped_lock(*mutex);
		}
	}

	DataModel::Lock::~Lock()
	{
		delete lock;
	}
}
