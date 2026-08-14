#include "v8datamodel/TimerService.h"
#include "util/standardout.h"

namespace RBX
{
	TimerService::TimerService() {}

	void TimerService::delay(boost::function0<void> func, double seconds)
	{
		Item item;
		item.time = G3D::System::getLocalTime() + seconds;
		item.func = func;

		std::list<Item>::iterator pos = items.end();

		std::list<Item>::reverse_iterator iter = items.rbegin();
		std::list<Item>::reverse_iterator end = items.rend();

		for (; iter != end; iter++)
		{
			if (iter->time > item.time)
				pos = iter.base();
			else
				break;
		}

		items.insert(pos, item);
	}

	void TimerService::onEvent(const RunService* source, Heartbeat event)
	{
		G3D::RealTime time = G3D::System::getLocalTime();

		while (items.size() > 0)
		{
			Item& item = items.front();

			if (time >= item.time)
			{
				try
				{
					item.func();
				}
				catch (std::exception& e)
				{
					StandardOut::singleton()->print(MESSAGE_WARNING, e);
				}

				items.pop_front();
			}
			else
			{
				break;
			}
		}
	}

	void TimerService::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Notifier<RunService, Heartbeat>::disconnect(runService, this);

		Instance::onServiceProvider(oldProvider, newProvider);
		runService = shared_from(ServiceProvider::create<RunService>(newProvider));

		Notifier<RunService,Heartbeat>::connect(runService, this);
	}
}
