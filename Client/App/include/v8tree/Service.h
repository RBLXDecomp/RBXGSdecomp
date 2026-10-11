#pragma once
#include "v8tree/Instance.h"

namespace RBX
{
	class Service
	{
	};

	// EVENTS
	class Closing
	{
	};

	struct ServiceAdded
	{
	public:
		const boost::shared_ptr<Instance> service;
	  
	public:
		ServiceAdded(const ServiceAdded& other)
			: service(other.service)
		{
		}
		ServiceAdded(Instance* instance)
			: service(shared_from(instance))
		{
		}

	private:
		ServiceAdded& operator=(const ServiceAdded&);
	};

	struct ServiceRemoving
	{
	public:
		const boost::shared_ptr<Instance> service;
	  
	public:
		ServiceRemoving(const ServiceRemoving& other)
			: service(other.service)
		{
		}
		ServiceRemoving(Instance* instance)
			: service(shared_from(instance))
		{
		}

	private:
		ServiceRemoving& operator=(const ServiceRemoving&);
	};

	extern const char* const sServiceProvider;
	class ServiceProvider : public DescribedNonCreatable<ServiceProvider, Instance, &sServiceProvider>,
							public Notifier<ServiceProvider, Closing>,
							public Notifier<ServiceProvider, ServiceAdded>,
							public Notifier<ServiceProvider, ServiceRemoving>
	{
	private:
		mutable std::vector<boost::shared_ptr<Instance>> serviceArray;
		std::map<const Name*, boost::shared_ptr<Instance>> serviceMap;

	private:
		static Reflection::BoundFuncDesc<ServiceProvider, boost::shared_ptr<Instance>(std::string), 1> func_service;
		static Reflection::BoundFuncDesc<ServiceProvider, boost::shared_ptr<Instance>(std::string), 1> func_GetService;
	  
	protected:
		virtual boost::shared_ptr<Instance> createChild(const Name&);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);
		virtual void onDescendentAdded(Instance*);
		virtual void onChildAdded(Instance*);
		virtual void onChildRemoving(Instance*);
		virtual void onAddListener(Listener<ServiceProvider, ServiceAdded>*) const;
		virtual bool askAddChild(const Instance*) const;
		void clearServices();

	private:
		boost::shared_ptr<Instance> findServiceByClassName(const Name&) const;
		boost::shared_ptr<Instance> findServiceByClassNameString(std::string);
	  
	public:
		static Instance* create(Instance*, const Name&);
		// TODO: check if this function inlines everywhere correctly
		static const ServiceProvider* findServiceProvider(const Instance* context)
		{
			while (context != NULL)
			{
				if (const ServiceProvider* s = dynamic_cast<const ServiceProvider*>(context))
					return s;

				context = context->getParent();
			}

			return NULL;
		}

	private:
		static size_t newIndex();

	public:
		template<typename Class>
		Class* find() const
		{
			size_t index = getClassIndex<Class>();
			if (index + 1 > serviceArray.size())
			{
				serviceArray.resize(index + 1);
			}
			else
			{
				Class* service = static_cast<Class*>(serviceArray[index].get());
				if (service)
					return service;
			}

			static const bool isNullClassName = (Class::className() == Name::getNullName());
			if (isNullClassName)
				return NULL;

			boost::shared_ptr<Instance> i = findServiceByClassName(Class::className());
			if (!i)
				return NULL;

			Class* service = static_cast<Class*>(i.get());
			serviceArray[index] = i;
			return service;
		}
	
		template<typename Class>
		Class* create() const
		{
			Class* foundService = this->find<Class>();
			if (!foundService)
			{
				boost::shared_ptr<Class> s = Creatable::create<Class>();
				foundService = s.get();

				serviceArray[getClassIndex<Class>()] = s;
				foundService->setParent(const_cast<ServiceProvider*>(this));

				// TODO: check Lighting template separately as it is part of code compiled with LTO
				RBXASSERT(Class::className() == Name::getNullName() || serviceMap.find(&Class::className()) != serviceMap.end());
			}

			return foundService;
		}

		template<typename Class>
		static Class* find(const Instance* context)
		{
			while (context != NULL)
			{
				if (const ServiceProvider* s = dynamic_cast<const ServiceProvider*>(context))
					return s->find<Class>();

				context = context->getParent();
			}

			return NULL;
		}

		template<typename Class>
		static Class* create(const Instance* context)
		{
			while (context != NULL)
			{
				if (const ServiceProvider* s = dynamic_cast<const ServiceProvider*>(context))
					return s->create<Class>();

				context = context->getParent();
			}

			return NULL;
		}

		template<typename Class>
		static Class* find(const ServiceProvider* serviceProvider)
		{
			if (serviceProvider)
				return serviceProvider->find<Class>();
			return NULL;
		}

		template<typename Class>
		static Class* create(const ServiceProvider* serviceProvider)
		{
			if (serviceProvider)
				return serviceProvider->create<Class>();
			return NULL;
		}
	
	private:
		template<typename Class>
		static size_t doGetClassIndex()
		{
			static size_t index = newIndex();
			return index;
		}
		
		template<typename Class>
		static void callDoGetClassIndex()
		{
			doGetClassIndex<Class>();
		}

		template<typename Class>
		static size_t getClassIndex()
		{
			static boost::once_flag flag = BOOST_ONCE_INIT;
			boost::call_once(&callDoGetClassIndex<Class>, flag);

			return doGetClassIndex<Class>();
		}
	};

	template<typename Class>
	class ServiceClient
	{
	private:
		Instance* context;
		mutable boost::shared_ptr<Class> service;

	public:
		ServiceClient(Instance* context)
			: context(context)
		{
		}

		bool isNull() const;
		operator Class*()
		{
			return createService(true);
		}
		operator const Class*() const
		{
			return createService(true);
		}

		Class* operator->()
		{
			return createService(true);
		}

		const Class* operator->() const
		{
			return createService(true);
		}

	private:
		Class* findService(bool) const;
		Class* createService(bool createIfNotFound) const
		{
			if (service == NULL)
				service = shared_from<Class>(ServiceProvider::create<Class>(context));

			return service.get();
		}
	};
}
