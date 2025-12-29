#pragma once
#include <boost/shared_ptr.hpp>
#include <map>
#include "util/Name.h"

namespace RBX
{
	// TODO: check if matches
	template<typename Class>
	boost::shared_ptr<Class> shared_from(Class* r)
	{
		return boost::shared_static_cast<Class>(r->shared_from_this());
	}

	class Object
	{
	public:
		virtual ~Object() {} //TODO:check
		Object(const Object&);
		Object();
	public:
		//Object& operator=(const Object&);
	};

	class ICreator
	{
	public:
		virtual boost::shared_ptr<Object> create() const = 0;
	public:
		//ICreator(const ICreator&);
		ICreator();
	public:
		//ICreator& operator=(const ICreator&);
	};

	template<typename T>
	class Creatable : public Object
	{
	public:
		class Deleter
		{
		public:
			void operator()(T*);
		};

	public:
		//Creatable(const Creatable&);
	protected:
		Creatable();
		virtual ~Creatable() {} // TODO:check
	public:
		//Creatable& operator=(const Creatable&);

	public:
		template<typename Class>
		static boost::shared_ptr<Class> create()
		{
			return boost::shared_ptr<Class>(new Class(), Deleter());
		}
		template<typename Class, typename Parameter1>
		static boost::shared_ptr<Class> create(Parameter1 p1)
		{
			return boost::shared_ptr<Class>(new Class(p1), Deleter());
		}
		template<typename Class, typename Parameter1, typename Parameter2>
		static boost::shared_ptr<Class> create(Parameter1 p1, Parameter2 p2)
		{
			return boost::shared_ptr<Class>(new Class(p1, p2), Deleter());
		}
	private:
		static void* operator new(size_t size)
		{
			return malloc(size);
		}
		static void operator delete(void* ptr)
		{
			free(ptr);
		}
	};

	template<typename T>
	class AbstractFactoryProduct : public Creatable<T>
	{
	public:
		//AbstractFactoryProduct(const AbstractFactoryProduct&);
	protected:
		AbstractFactoryProduct();
	public:
		virtual const Name& getClassName() const;
	public:
		virtual ~AbstractFactoryProduct() {} //todo:check
	public:
		//AbstractFactoryProduct& operator=(const AbstractFactoryProduct&);
	  
	protected:
		static std::map<const Name*, const ICreator*>& getCreators()
		{
			static std::map<const Name*, const ICreator*> creators;
			return creators;
		}
	public:
		static boost::shared_ptr<T> create(const Name& name)
		{
			std::map<const Name*, const ICreator*>::iterator iter = getCreators().find(&name);
			if (iter == getCreators().end())
			{
				return boost::shared_ptr<T>();
			}
			else
			{
				return boost::shared_dynamic_cast<T>(iter->second->create());
			}
		}
	};

	template<typename Class, typename DerivedClass, const char** ClassName>
	class FactoryProduct : public DerivedClass
	{
	private:
		class Creator : public ICreator
		{
		public:
			virtual boost::shared_ptr<Object> create() const;
			const Name& getClassName() const;
		public:
			//Creator(const Creator&);
			Creator();
			~Creator();
		public:
			//Creator& operator=(const Creator&);
		};

	private:
		static const Creator creator;
	  
	public:
		//FactoryProduct(const FactoryProduct&);
	protected:
		FactoryProduct();
		virtual ~FactoryProduct();
	public:
		const ICreator& getCreator()
		{
			return creator;
		}
		virtual const Name& getClassName() const;
		//FactoryProduct& operator=(const FactoryProduct&);
	  
	public:
		static const Name& className();
	};

	template<typename DerivedClass, const char** ClassName>
	class NonFactoryProduct : public DerivedClass
	{
	public:
		//NonFactoryProduct(const NonFactoryProduct&);
		NonFactoryProduct()
			: DerivedClass()
		{
		}
	public:
		virtual const Name& getClassName() const;
	public:
		virtual ~NonFactoryProduct();
	public:
		//NonFactoryProduct& operator=(const NonFactoryProduct&);

	public:
		static const Name& className();
	};
}
