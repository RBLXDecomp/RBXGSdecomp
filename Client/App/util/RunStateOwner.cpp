#include "util/RunStateOwner.h"
#include "util/Timer.h"
#include "util/standardout.h"
#include "v8datamodel/DataModel.h"
#include <boost/thread/xtime.hpp>

namespace RBX
{
	const char* const sRunService = "RunService";

	static Reflection::BoundFuncDesc<RunService, void(void), 0> runFunction(&RunService::run, "Run", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<RunService, void(void), 0> pauseFunction(&RunService::pause, "Pause", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<RunService, void(void), 0> resetFunction(&RunService::reset, "Reset", Reflection::FunctionDescriptor::NeedTrustedCaller);

	static Reflection::SignalDesc<RunService, void(float)> event_Heartbeat("Heartbeat", "interval");
	Reflection::SignalDesc<RunService, void(float, float)> RunService::event_Stepped("Stepped", "time", "step");

	RunService::RunService()
		: framePeriod(1.0f/30.0f),
		  runState(RS_NORMAL),
		  invalidRunViewCount(0),
		  stopRequested(false),
		  runDisabled(false)
	{
		setName("Run Service");
	}

	RunService::~RunService()
	{
		RBXASSERT(views.empty());
	}

	void RunService::endRunThread(bool join)
	{
		stopRequested = true;
		stateChangedCondition.notify_all();

		if (join)
			runThread->join();
	}

	void RunService::onAncestorChanged(const AncestorChanged& event)
	{
		if (!getParent())
		{
			stopRequested = true;
			stateChangedCondition.notify_all();
		}
	}

	void RunService::setRunState(RunState newState)
	{
		RunTransition transition(runState, newState);
		runState = newState;

		Notifier<RunService, RunState>::raise(newState);
		Notifier<RunService, RunTransition>::raise(transition);
	}

	void RunService::raiseStepped(float time, float step)
	{
		Notifier<RunService, Stepped>::raise(Stepped(time, step));
		event_Stepped.fire(this, time, step);
	}

	void RunService::raiseHeartbeat(float time, float step)
	{
		Notifier<RunService, Heartbeat>::raise(Heartbeat(time, step));
		event_Heartbeat.fire(this, step);
	}

	void RunService::invalidateRunViews()
	{
		boost::mutex::scoped_lock lock(viewMutex);

		if (invalidRunViewCount == 0)
		{
			invalidRunViewCount = (int)views.size();
			if (invalidRunViewCount > 0)
			{
				for (std::map<IRunView*, bool>::iterator iter = views.begin(); iter != views.end(); iter++)
				{
					iter->first->InvalidateRunView();
					iter->second = false;
				}
				runViewsDoneCondition.notify_all();
			}
		}
	}

	void RunService::start()
	{
		RBXASSERT(!runThread);

		boost::shared_ptr<DataModel> dataModel = shared_from(dynamic_cast<DataModel*>(getParent()));
		runThread.reset(new boost::thread(background_function(boost::bind(&RunService::runProc, shared_from(this), dataModel), "rbx_runProc")));
	}

	void RunService::runProc(boost::shared_ptr<DataModel> dataModel)
	{
		double lastTime = getRealTime();
		while (true)
		{
			double deltaTime = lastTime - getRealTime();

			{
				boost::mutex::scoped_lock lock(viewMutex);
				for (std::map<IRunView*, bool>::iterator it = views.begin(); it != views.end(); it++)
					it->first->AdjustThrottle(deltaTime);
			}

			double clampedDeltaTime = G3D::max(0.0, deltaTime);
			boost::xtime stateChangedTimeout;
			boost::xtime_get(&stateChangedTimeout, boost::TIME_UTC);
			stateChangedTimeout.nsec += (boost::xtime::xtime_nsec_t)(clampedDeltaTime * 1e9);

			bool stateChanged;
			{
				boost::mutex::scoped_lock lock(runMutex);
				stateChanged = stateChangedCondition.timed_wait(lock, stateChangedTimeout);
			}

			if (!stateChanged)
			{
				boost::xtime runViewsDoneTimeout;
				boost::xtime_get(&runViewsDoneTimeout, boost::TIME_UTC);
				runViewsDoneTimeout.sec += 1;

				boost::mutex::scoped_lock lock(runMutex);
				if (invalidRunViewCount != 0)
					runViewsDoneCondition.timed_wait(lock, runViewsDoneTimeout);
			}

			if (deltaTime <= 0.0)
				lastTime = getRealTime();

			{
				DataModel::Lock lock(dataModel);

				if (!stopRequested)
				{
					RBXASSERT(getParent() == dataModel.get());
					dataModel->step(framePeriod);
				}
				else
				{
					StandardOut::singleton()->print(MESSAGE_INFO, "runProc ending");
					return;
				}
			}

			lastTime += framePeriod;
		}
	}
}
