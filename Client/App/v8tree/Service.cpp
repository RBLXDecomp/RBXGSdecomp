#include "v8tree/Service.h"
#include <G3D/format.h>

namespace RBX
{
	const char* sServiceProvider = "ServiceProvider";

	Reflection::BoundFuncDesc<ServiceProvider, boost::shared_ptr<Instance>(std::string), 1> ServiceProvider::func_service(&ServiceProvider::findServiceByClassNameString, "service", "name", Reflection::FunctionDescriptor::AnyCaller);
	Reflection::BoundFuncDesc<ServiceProvider, boost::shared_ptr<Instance>(std::string), 1> ServiceProvider::func_GetService(&ServiceProvider::findServiceByClassNameString, "GetService", "name", Reflection::FunctionDescriptor::AnyCaller);

	ServiceProvider::ServiceProvider()
	{
	}

	ServiceProvider::~ServiceProvider()
	{
	}

	size_t ServiceProvider::newIndex()
	{
		static size_t index = 0xFFFFFFFF; // max unsigned integer
		return InterlockedIncrement((LONG*)&index);
	}

	void ServiceProvider::onChildAdded(Instance* instance)
	{
		RBXASSERT(Instance::fastDynamicCast<ServiceProvider>(instance)==NULL);

		if (Instance::fastDynamicCast<Service>(instance))
		{
			if (!instance->getClassName().empty())
			{
				RBXASSERT(!findServiceByClassName(instance->getClassName()));
				serviceMap[&instance->getClassName()] = shared_from(instance);
			}
			Notifier<ServiceProvider, ServiceAdded>::raise(ServiceAdded(instance));
		}
	}

	void ServiceProvider::onChildRemoving(Instance* instance)
	{
		std::map<const Name*, boost::shared_ptr<Instance>>::const_iterator it = serviceMap.find(&instance->getClassName());
		if (it != serviceMap.end())
			Notifier<ServiceProvider, ServiceRemoving>::raise(ServiceRemoving(instance));
	}

	void ServiceProvider::onDescendentAdded(Instance* instance)
	{
		Instance::onDescendentAdded(instance);
		instance->onServiceProvider(NULL, this);
	}

	void ServiceProvider::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		instance->onServiceProvider(this, NULL);
		Instance::onDescendentRemoving(instance);
	}

	void ServiceProvider::onAddListener(Listener<ServiceProvider, ServiceAdded>* listener) const
	{
		typedef std::vector<boost::shared_ptr<Instance>>::const_iterator Iterator;

		Iterator iter = serviceArray.begin();
		Iterator end = serviceArray.end();
		for (; iter != end; iter++)
		{
			if (fastDynamicCast<Service>((*iter).get()))
			{
				Notifier<ServiceProvider, ServiceAdded>::raise(ServiceAdded((*iter).get()), listener);
			}
		}
	}

	void ServiceProvider::clearServices()
	{
		RBXASSERT(numChildren()==0);
		serviceArray.clear();
		serviceMap.clear();
	}

	boost::shared_ptr<Instance> ServiceProvider::createChild(const Name& className)
	{
		RBXASSERT(className!=RBX::Name::getNullName());

		boost::shared_ptr<Instance> instance = findServiceByClassName(className);
		if (instance)
		{
			return instance;
		}

		return AbstractFactoryProduct<Instance>::create(className);
	}

	boost::shared_ptr<Instance> ServiceProvider::findServiceByClassName(const Name& className) const
	{
		RBXASSERT(className!=RBX::Name::getNullName());

		std::map<const Name*, boost::shared_ptr<Instance>>::const_iterator it = serviceMap.find(&className);
		if (it != serviceMap.end())
			return (*it).second;

		return boost::shared_ptr<Instance>();
	}

	boost::shared_ptr<Instance> ServiceProvider::findServiceByClassNameString(std::string sName)
	{
		const Name& name = Name::lookup(sName);
		if (name.empty())
			throw std::runtime_error(G3D::format("'%s' is not a valid Service name", sName.c_str()));

		return RBX::shared_from(create(this, name));
	}

	Instance* ServiceProvider::create(Instance* context, const Name& name)
	{
		ServiceProvider* provider = Instance::findFirstAncestorOfClass<ServiceProvider>(context);
		if (!provider)
			return NULL;

		boost::shared_ptr<Instance> child = provider->createChild(name);
		if (child)
			child->setParent(provider);

		return child.get();
	}

	// See: DataModel::askAddChild
	// ServiceProvider likely has an implementation similar to this, but DataModel just reimplemented it in exactly the same way
	// Many other ServiceProviders have implementations like this
	// TODO: check match
	bool ServiceProvider::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const Service>(instance) != NULL;
	}
}
