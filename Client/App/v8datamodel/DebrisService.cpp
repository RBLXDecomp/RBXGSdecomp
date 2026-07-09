#include "v8datamodel/DebrisService.h"
#include "v8datamodel/TimerService.h"
#include "util/standardout.h"

static RBX::Reflection::BoundFuncDesc<RBX::DebrisService, void(boost::shared_ptr<RBX::Instance>, double), 2> func_AddItem(&RBX::DebrisService::addItem, "AddItem", "item", "lifetime", RBX::Reflection::FunctionDescriptor::AnyCaller);
static RBX::Reflection::PropDescriptor<RBX::DebrisService, int> prop_MaxItems("MaxItems", "Data", &RBX::DebrisService::getMaxItems, &RBX::DebrisService::setMaxItems, RBX::Reflection::PropertyDescriptor::STANDARD);

static void cleanup(boost::weak_ptr<RBX::Instance> item)
{
	try
	{
		boost::shared_ptr<RBX::Instance> i = item.lock();
		if (i)
			i->setParent(NULL);
	}
	catch (std::exception& e)
	{
		RBX::StandardOut::singleton()->print(RBX::MESSAGE_WARNING, e);
	}
}

namespace RBX
{
	DebrisService::DebrisService()
		: Base("Debris"),
		  maxItems(300),
		  timer(NULL)
	{
	}

	void DebrisService::setMaxItems(int value)
	{
		if (value != maxItems)
		{
			if (value < 0)
				throw std::runtime_error("DebrisService MaxItems must be greater than 0");

			maxItems = value;
			raisePropertyChanged(prop_MaxItems);
			DebrisService::cleanup();
		}
	}

	void DebrisService::addItem(boost::shared_ptr<Instance> item, double lifetime)
	{
		if (timer)
			timer->delay(boost::bind(&::cleanup, item), lifetime);

		queue.push(item);
		DebrisService::cleanup();
	}

	void DebrisService::cleanup()
	{
		while ((int)queue.size() > maxItems)
		{
			::cleanup(queue.front());
			queue.pop();
		}
	}

	void DebrisService::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		while ((int)queue.size() > 0)
		{
			::cleanup(queue.front());
			queue.pop();
		}

		Instance::onServiceProvider(oldProvider, newProvider);

		timer = newProvider ? newProvider->create<TimerService>() : NULL;
	}
}
