#pragma once
#include "reflection/object.h"

class ArchiveBinder;

namespace RBX
{
	class IReferenceBinder;

	// NOTE: may not be intended for this file
	class IIDREF
	{
		friend class MergeBinder;
		friend class ::ArchiveBinder;

	private:
		virtual void assignIDREF(Reflection::DescribedBase*, const InstanceHandle&) const = 0;
	};

	namespace Reflection
	{
		template<typename Class, const char** ClassName, typename DerivedClass>
		class __declspec(novtable) Described : public DerivedClass
		{
		public:
			// helps with constructors
			typedef Described<Class, ClassName, DerivedClass> Base;

		public:
			Described()
				: DerivedClass()
			{
				this->descriptor = &classDescriptor();
			}
			template<typename Arg0Type>
			Described(Arg0Type arg0)
				: DerivedClass(arg0)
			{
				this->descriptor = &classDescriptor();
			}
		  
		public:
			static const type_info& classType();
			static const type_info& baseClassType();
			static ClassDescriptor& classDescriptor()
			{
				// TODO: match (*ClassName is wrong)
				static ClassDescriptor foo(DerivedClass::classDescriptor(), *ClassName);
				return foo;
			}
		};
	}

	template<typename Class, typename DerivedClass, const char** ClassName>
	class DescribedCreatable : public Reflection::Described<Class, ClassName, FactoryProduct<Class, DerivedClass, ClassName>>
	{
	public:
		// helps with constructors
		typedef DescribedCreatable<Class, DerivedClass, ClassName> Base;

	public:
		//DescribedCreatable(const DescribedCreatable&);
	protected:
		DescribedCreatable();
		template<typename Arg0Type>
		DescribedCreatable(Arg0Type arg0);
	public:
		virtual ~DescribedCreatable();
	public:
		//DescribedCreatable& operator=(const DescribedCreatable&);
	};

	template<typename Class, typename DerivedClass, const char** ClassName>
	class DescribedNonCreatable : public Reflection::Described<Class, ClassName, NonFactoryProduct<DerivedClass, ClassName>>
	{
	public:
		//DescribedNonCreatable(const DescribedNonCreatable&);
	protected:
		DescribedNonCreatable()
			: Described()
		{
		}

		template<typename Arg0Type>
		DescribedNonCreatable(Arg0Type arg0)
			: Described(arg0)
		{
		}
	public:
		virtual ~DescribedNonCreatable();
	public:
		//DescribedNonCreatable& operator=(const DescribedNonCreatable&);
	};

	namespace Reflection
	{
		// BoundProp
		// Unknown might be the number of arguments, which would make no sense in this case
		template<typename PropType, int Unknown>
		class BoundProp : public TypedPropertyDescriptor<PropType>
		{
		public:
			template<typename Class>
			class BoundPropGetSet : public GetSet
			{
			private:
				BoundProp& desc;
				PropType (Class::*member);
				void (Class::*changed)(const PropertyDescriptor&);
			  
			public:
				//BoundPropGetSet(const BoundPropGetSet&);
				BoundPropGetSet(BoundProp& desc, PropType (Class::*member), void (Class::*changed)(const PropertyDescriptor&))
					: desc(desc),
					  member(member),
					  changed(changed)
				{
				}
			public:
				virtual bool isReadOnly() const
				{
					return false;
				}
				virtual PropType getValue(const DescribedBase* object) const
				{
					Class* o = (Class*)object;
					return o->*member;
				}
				virtual void setValue(DescribedBase* object, const PropType& value) const
				{
					Class* o = (Class*)object;
					if (o->*member != value)
					{
						o->*member = value;
						if (changed)
							(o->*changed)(desc);
						o->raisePropertyChanged(desc);
					}
				}
			};

		public:
			//BoundProp(BoundProp&);
			template<typename Class>
			BoundProp(const char* name, const char* category, PropType (Class::*member), Functionality flags)
				: TypedPropertyDescriptor(Class::classDescriptor(), name, category, std::auto_ptr<GetSet>(), flags)
			{
				getset.reset(new BoundPropGetSet<Class>(*this, member, NULL));
			}
			virtual ~BoundProp();
		public:
			//BoundProp& operator=(BoundProp&);
		};

		// PropDescriptor
		template<typename Class, typename ReturnType>
		class PropDescriptor : public TypedPropertyDescriptor<ReturnType>
		{
		private:
			// Get only
			template<typename GetFunction>
			class GetImpl : public GetSet
			{
			private:
				GetFunction get;
			  
			public:
				//GetImpl(const GetImpl&);
				GetImpl(GetFunction get)
					: GetSet(),
					  get(get)
				{
				}
			public:
				virtual bool isReadOnly() const
				{
					return true;
				}
				virtual ReturnType getValue(const DescribedBase* object) const
				{
					Class* o = (Class*)object;
					return (o->*get)();
				}
				virtual void setValue(DescribedBase* object, const ReturnType& value) const
				{
					throw std::runtime_error("can't set value");
				}
				//GetImpl& operator=(const GetImpl&);
			};

			// Set only
			template<typename SetFunction>
			class SetImpl : public GetSet
			{
			private:
				SetFunction set;
			  
			public:
				//SetImpl(const SetImpl&);
				SetImpl(SetFunction set)
					: GetSet(),
					  set(set)
				{
				}
			public:
				virtual bool isReadOnly() const
				{
					return false;
				}
				virtual ReturnType getValue(const DescribedBase* object) const
				{
					throw std::runtime_error("can't get value");
				}
				virtual void setValue(DescribedBase* object, const ReturnType& value) const
				{
					Class* o = (Class*)object;
					(o->*set)(value);
				}
				//SetImpl& operator=(const SetImpl&);
			};

			// Get & Set
			template<typename GetFunction, typename SetFunction>
			class GetSetImpl : public GetSet
			{
			private:
				GetFunction get;
				SetFunction set;
			  
			public:
				//GetSetImpl(const GetSetImpl&);
				GetSetImpl(GetFunction get, SetFunction set)
					: GetSet(),
					  get(get),
					  set(set)
				{
				}
			public:
				virtual bool isReadOnly() const
				{
					return false;
				}
				virtual ReturnType getValue(const DescribedBase* object) const
				{
					Class* o = (Class*)object;
					return (o->*get)();
				}
				virtual void setValue(DescribedBase* object, const ReturnType& value) const
				{
					Class* o = (Class*)object;
					(o->*set)(value);
				}
				//GetSetImpl& operator=(const GetSetImpl&);
			};

		public:
			//PropDescriptor(PropDescriptor&);
			template<typename GetFunction, typename SetFunction>
			PropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags)
				: TypedPropertyDescriptor(Class::classDescriptor(), name, category, getset(get, set), flags)
			{
			}
			virtual ~PropDescriptor();
		public:
			//PropDescriptor& operator=(PropDescriptor&);

		public:
			// Note: int indicates that the input value is NULL
			template<typename GetFunction>
			static std::auto_ptr<GetSet> getset(GetFunction get, int set)
			{
				return std::auto_ptr<GetSet>(new GetImpl<GetFunction>(get));
			}

			template<typename SetFunction>
			static std::auto_ptr<GetSet> getset(int get, SetFunction set)
			{
				return std::auto_ptr<GetSet>(new SetImpl<SetFunction>(set));
			}

			template<typename GetFunction, typename SetFunction>
			static std::auto_ptr<GetSet> getset(GetFunction get, SetFunction set)
			{
				return std::auto_ptr<GetSet>(new GetSetImpl<GetFunction, SetFunction>(get, set));
			}
		};

		// RefPropDescriptor
		template<typename Class, typename ReturnType>
		class RefPropDescriptor : public RefPropertyDescriptor, public IIDREF
		{
		private:
			std::auto_ptr<typename TypedPropertyDescriptor<ReturnType*>::GetSet> getset;
		  
		public:
			//RefPropDescriptor(RefPropDescriptor&);
		public:
			virtual bool isReadOnly() const;
			ReturnType* getValue(const DescribedBase*) const;
			void setValue(DescribedBase*, ReturnType*) const;
			virtual bool equalValues(const DescribedBase*, const DescribedBase*) const;
			virtual DescribedBase* getRefValue(const DescribedBase*) const;
			virtual void setRefValue(DescribedBase*, DescribedBase*) const;
			virtual void readValue(DescribedBase*, const XmlElement*, IReferenceBinder&) const;
			virtual void writeValue(const DescribedBase*, XmlElement*) const;
			virtual void assignIDREF(DescribedBase*, const InstanceHandle&) const;
		public:
			template<typename GetFunction, typename SetFunction>
			RefPropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags)
				: RefPropertyDescriptor(Class::classDescriptor(), RefType::singleton<Class*>(), name, category, flags),
				  IIDREF(),
				  getset(PropDescriptor<Class, ReturnType*>::getset(get, set))
			{
			}
			virtual ~RefPropDescriptor();
		public:
			//RefPropDescriptor& operator=(RefPropDescriptor&);
		};

		// EnumPropDescriptor
		template<typename Class, typename Enum>
		class EnumPropDescriptor : public EnumPropertyDescriptor
		{
		private:
			std::auto_ptr<typename TypedPropertyDescriptor<Enum>::GetSet> getset;
		  
		public:
			//EnumPropDescriptor(EnumPropDescriptor&);
			template<typename GetFunction, typename SetFunction>
			EnumPropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags);
		public:
			virtual bool isReadOnly() const;
			Enum getValue(const DescribedBase*) const;
			void setValue(DescribedBase*, Enum) const;
			virtual bool equalValues(const DescribedBase*, const DescribedBase*) const;
			virtual int getEnumValue(const DescribedBase*) const;
			virtual bool setEnumValue(DescribedBase*, int) const;
			virtual unsigned getIndexValue(const DescribedBase*) const;
			virtual bool setIndexValue(DescribedBase*, unsigned) const;
			virtual bool hasStringValue() const;
			virtual std::string getStringValue(const DescribedBase*) const;
			virtual bool setStringValue(DescribedBase*, const Name&) const;
			virtual bool setStringValue(DescribedBase*, const std::string&) const;
			virtual void readValue(DescribedBase*, const XmlElement*, IReferenceBinder&) const;
			virtual void writeValue(const DescribedBase*, XmlElement*) const;
		public:
			virtual ~EnumPropDescriptor();
		public:
			//EnumPropDescriptor& operator=(EnumPropDescriptor&);
		};

		// BoundFuncDesc
		template<typename Class, typename Function, int ArgCount>
		class BoundFuncDesc;

		// Specialised BoundFuncDesc implementations for different argument counts
		// Zero arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 0> : public FuncDesc<Class>
		{
		private:
			typedef FunctionTraits<Function> Traits;

		public:
			typedef typename Traits::ReturnType(Class::*FunctionSig)();

		private:
			typename FunctionSig function;
  
		private:
			void declareSignature()
			{
				signature.resultType = &Type::singleton<typename Traits::ReturnType>();
			}

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				Security security)
				: FuncDesc(name, security),
				  function(function)
			{
				declareSignature();
			}
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Class* o = dynamic_cast<Class*>(instance);
				if (!o)
					throw std::bad_cast();

				call<typename Traits::ReturnType>(o, arguments.returnValue);
			}

		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue) const
			{
				returnValue.set<ReturnType>((o->*function)());
			}

			template<>
			void call<void>(Class* o, Value& returnValue) const
			{
				(o->*function)();
			}
		
		public:
			virtual ~BoundFuncDesc() {}
		public:
			//BoundFuncDesc& operator=(const BoundFuncDesc&);
		};

		// One argument
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 1> : public FuncDesc<Class>
		{
		private:
			typedef FunctionTraits<Function> Traits;

		public:
			typedef typename Traits::ReturnType(Class::*FunctionSig)(typename Traits::Arg1Type);

		private:
			typename FunctionSig function;
			Value default1;
  
		private:
			void declareSignature(const char* arg1Name)
			{
				signature.resultType = &Type::singleton<typename Traits::ReturnType>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Traits::Arg1Type>(), default1);
			}

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1()
			{
				declareSignature(arg1Name);
			}
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Traits::Arg1Type default1,
				Security security);
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);

				Class* o = dynamic_cast<Class*>(instance);
				if (!o)
					throw std::bad_cast();

				call<typename Traits::ReturnType>(o, arguments.returnValue, arg1);
			}

		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Traits::Arg1Type>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1) const
			{
				(o->*function)(arg1.convert<typename Traits::Arg1Type>());
			}
		
		public:
			virtual ~BoundFuncDesc() {}
		public:
			//BoundFuncDesc& operator=(const BoundFuncDesc&);
		};

		// Two arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 2> : public FuncDesc<Class>
		{
		private:
			typedef FunctionTraits<Function> Traits;

		public:
			typedef typename Traits::ReturnType(Class::*FunctionSig)(typename Traits::Arg1Type, typename Traits::Arg2Type);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name)
			{
				signature.resultType = &Type::singleton<typename Traits::ReturnType>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Traits::Arg1Type>(), default1);
				signature.addArgument(Name::declare(arg2Name, -1), Type::singleton<typename Traits::Arg2Type>(), default2);
			}

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				Security security);
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1(),
				  default2(default2)
			{
				declareSignature(arg1Name, arg2Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Traits::Arg1Type default1,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				Security security);
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);
				Value arg2 = default2;
				arguments.get(2, arg2);

				Class* o = dynamic_cast<Class*>(instance);
				if (!o)
					throw std::bad_cast();

				call<typename Traits::ReturnType>(o, arguments.returnValue, arg1, arg2);
			}
		
		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1, Value& arg2) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Traits::Arg1Type>(), arg2.convert<typename Traits::Arg2Type>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1, Value& arg2) const
			{
				(o->*function)(arg1.convert<typename Traits::Arg1Type>(), arg2.convert<typename Traits::Arg2Type>());
			}

		public:
			virtual ~BoundFuncDesc() {}
		public:
			//BoundFuncDesc& operator=(const BoundFuncDesc&);
		};

		// Three arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 3> : public FuncDesc<Class>
		{
		private:
			typedef FunctionTraits<Function> Traits;

		public:
			typedef typename Traits::ReturnType(Class::*FunctionSig)(typename Traits::Arg1Type, typename Traits::Arg2Type, typename Traits::Arg3Type);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
			Value default3;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name, const char* arg3Name);

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				Security security);
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				Security security);

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				Security security);

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Traits::Arg1Type default1,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				Security security);
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const;
		
		public:
			virtual ~BoundFuncDesc() {}
		public:
			//BoundFuncDesc& operator=(const BoundFuncDesc&);
		};

		// Four arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 4> : public FuncDesc<Class>
		{
		private:
			typedef FunctionTraits<Function> Traits;

		public:
			typedef typename Traits::ReturnType(Class::*FunctionSig)(typename Traits::Arg1Type, typename Traits::Arg2Type, typename Traits::Arg3Type, typename Traits::Arg4Type);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
			Value default3;
			Value default4;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name, const char* arg3Name, const char* arg4Name);

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				const char* arg4Name,
				Security security);
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				const char* arg4Name,
				typename Traits::Arg4Type default4,
				Security security);

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				const char* arg4Name,
				typename Traits::Arg4Type default4,
				Security security);

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				const char* arg4Name,
				typename Traits::Arg4Type default4,
				Security security);

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Traits::Arg1Type default1,
				const char* arg2Name,
				typename Traits::Arg2Type default2,
				const char* arg3Name,
				typename Traits::Arg3Type default3,
				const char* arg4Name,
				typename Traits::Arg4Type default4,
				Security security);
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const;
		
		public:
			virtual ~BoundFuncDesc() {}
		public:
			//BoundFuncDesc& operator=(const BoundFuncDesc&);
		};
	}
}
