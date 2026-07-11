#include "IdManager.h"
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace RBX
{

	void IdManager::addInstance(Instance* instance)
	{
		Guid::Data extractedData;
		instance->getGuid().extract(extractedData);

		RBXASSERT(getInstance(extractedData) == NULL || getInstance(extractedData) == instance);

		items[extractedData] = instance;
	}

	void IdManager::removeInstance(boost::shared_ptr<Instance> instance)
	{
		Guid::Data extractedData;
		instance->getGuid().extract(extractedData);

		items.erase(extractedData);
	}

	void IdManager::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		if (newProvider)
		{
			boost::slot<boost::function<void(boost::shared_ptr<Instance>)>> slot(boost::bind(&IdManager::removeInstance, this, _1));
			removeInstanceConnection = event_descendentRemoving.connect(getRootAncestor(), slot);
		}
	}
}
