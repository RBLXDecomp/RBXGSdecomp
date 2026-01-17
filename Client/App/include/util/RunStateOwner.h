#pragma once
#include "reflection/reflection.h"
#include "reflection/signal.h"
#include "util/Events.h"
#include "v8tree/Service.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>

namespace RBX
{
	extern const char* sRunService;

	class DataModel;

	enum RunState
	{
		RS_NORMAL,
		RS_RUNNING,
		RS_PAUSED
	};

	class RunTransition
	{
	public:
		RunState oldState;
		RunState newState;

		RunTransition(RunState oldState, RunState newState)
			: oldState(oldState),
			newState(newState)
		{
		}

		bool startEvent();
		bool resetEvent();
	};

	class Heartbeat
	{
	public:
		const float time;
		const float step;

		Heartbeat(float time, float step)
			: time(time),
			step(step)
		{
		}
	};

	class Stepped
	{
	public:
		const float time;
		const float step;

		Stepped(float time, float step)
			: time(time),
			step(step)
		{
		}
	};

	class IRunView
	{
	public:
		virtual void InvalidateRunView() = 0;
		virtual void AdjustThrottle(double) = 0;
		//IRunView(const IRunView&);
		IRunView();
		//IRunView& operator=(const IRunView&);
	};

	class RunService : public DescribedCreatable<RunService, Instance, &sRunService>,
					   public Service,
					   public Notifier<RunService, Heartbeat>,
					   public Notifier<RunService, Stepped>,
					   public Notifier<RunService, RunTransition>,
					   public Notifier<RunService, RunState>
	{
	private:
		RunState runState;
		boost::mutex runMutex;
		bool stopRequested;
		boost::condition runViewsDoneCondition;
		boost::condition stateChangedCondition;
		float framePeriod;
		boost::scoped_ptr<boost::thread> runThread;
		boost::mutex viewMutex;
		std::map<IRunView*, bool> views;
		int invalidRunViewCount;
	public:
		bool runDisabled;
		static Reflection::SignalDesc<RunService, void(float, float)> event_Stepped;
	private:
		void start();
	public:
		//RunService(const RunService&);
		RunService();
		virtual ~RunService();
		void setRunState(RunState newState);
		void run();
		void pause();
		void reset();
		void endRunThread(bool join);
		void raiseHeartbeat(float time, float step);
		void raiseStepped(float time, float step);
		RunState getRunState() const;
		bool isEditState() const;
		bool isRunState() const;
		bool isPauseState() const;
		void setPeriod(float);
		float getPeriod();
		void addRunView(IRunView*);
		void removeRunView(IRunView*);
		void invalidateRunViews();
		void runViewValid(IRunView*);
		virtual void onAncestorChanged(const AncestorChanged& event);
	private:
		void runProc(boost::shared_ptr<DataModel> dataModel);
	public:
		//RunService& operator=(const RunService&);
	};

}
