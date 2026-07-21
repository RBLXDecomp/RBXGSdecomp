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
		ServiceAdded(const ServiceAdded&);
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
		ServiceRemoving(const ServiceRemoving&);
		ServiceRemoving(Instance* instance)
			: service(shared_from(instance))
		{
		}

	private:
		ServiceRemoving& operator=(const ServiceRemoving&);
	};

	extern const char* sServiceProvider;
	class __declspec(novtable) ServiceProvider : public DescribedNonCreatable<ServiceProvider, Instance, &sServiceProvider>,
							public Notifier<ServiceProvider, Closing>,
							public Notifier<ServiceProvider, ServiceAdded>,
							public Notifier<ServiceProvider, ServiceRemoving>
	{
	private:
		std::vector<boost::shared_ptr<Instance>> serviceArray;
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
				if (const ServiceProvider* s = fastDynamicCast<const ServiceProvider>(context))
					return s;

				context = context->getParent();
			}

			return NULL;
		}

	private:
		static size_t newIndex();

	public:
		template<typename Class>
		Class* find() const;
	
		template<typename Class>
		Class* create() const;

		template<typename Class>
		static Class* find(const Instance* context);

		template<typename Class>
		static Class* create(const Instance* context);
	
	private:
		template<typename Class>
		static size_t doGetClassIndex();
		
		template<typename Class>
		static void callDoGetClassIndex();
	};

	template<typename Class>
	class ServiceClient
	{
	private:
		Instance* context;
		boost::shared_ptr<Class> service;

	public:
		ServiceClient(Instance* context)
			: context(context)
		{
		}

		bool isNull() const;
		operator Class*();
		operator const Class*() const;

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
		Class* createService(bool) const;
	};
}
