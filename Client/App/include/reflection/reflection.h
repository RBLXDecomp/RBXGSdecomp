#pragma once
#include <boost/type_traits.hpp>
#include <boost/cast.hpp>
#include "reflection/object.h"

class ArchiveBinder;

namespace RBX
{
	class IReferenceBinder;

	// NOTE: may not be intended for this file
	class __declspec(novtable) IIDREF
	{
		friend class MergeBinder;
		friend class ::ArchiveBinder;

	private:
		virtual void assignIDREF(Reflection::DescribedBase*, const InstanceHandle&) const = 0;
	};

	namespace Reflection
	{
		template<typename Class, const char* const* ClassName, typename DerivedClass>
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
				static ClassDescriptor foo(DerivedClass::classDescriptor(), (const char*)ClassName);
				return foo;
			}
		};
	}

	template<typename Class, typename DerivedClass, const char* const* ClassName>
	class DescribedCreatable : public Reflection::Described<Class, ClassName, FactoryProduct<Class, DerivedClass, ClassName>>
	{
	public:
		// helps with constructors
		typedef DescribedCreatable<Class, DerivedClass, ClassName> Base;

	protected:
		DescribedCreatable()
			: Described()
		{
		}

		template<typename Arg0Type>
		DescribedCreatable(Arg0Type arg0)
			: Described(arg0)
		{
		}
	};

	template<typename Class, typename DerivedClass, const char* const* ClassName>
	class DescribedNonCreatable : public Reflection::Described<Class, ClassName, NonFactoryProduct<DerivedClass, ClassName>>
	{
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
			template<typename Class>
			BoundProp(const char* name, const char* category, PropType (Class::*member), Functionality flags)
				: TypedPropertyDescriptor(Class::classDescriptor(), name, category, std::auto_ptr<GetSet>(), flags)
			{
				getset.reset(new BoundPropGetSet<Class>(*this, member, NULL));
			}

			template<typename Class>
			BoundProp(const char* name, const char* category, PropType (Class::*member), void (Class::*changed)(const PropertyDescriptor&), Functionality flags)
				: TypedPropertyDescriptor(Class::classDescriptor(), name, category, std::auto_ptr<GetSet>(), flags)
			{
				getset.reset(new BoundPropGetSet<Class>(*this, member, changed));
			}
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
			};

			// Set only
			template<typename SetFunction>
			class SetImpl : public GetSet
			{
			private:
				SetFunction set;
			  
			public:
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
			};

			// Get & Set
			template<typename GetFunction, typename SetFunction>
			class GetSetImpl : public GetSet
			{
			private:
				GetFunction get;
				SetFunction set;
			  
			public:
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
			};

		public:
			template<typename GetFunction, typename SetFunction>
			PropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags)
				: TypedPropertyDescriptor(Class::classDescriptor(), name, category, getset(get, set), flags)
			{
			}

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
			virtual bool isReadOnly() const
			{
				return getset->isReadOnly();
			}
			ReturnType* getValue(const DescribedBase* object) const
			{
				return getset->getValue(object);
			}
			void setValue(DescribedBase* object, ReturnType* value) const
			{
				getset->setValue(object, value);
			}
			virtual bool equalValues(const DescribedBase* a, const DescribedBase* b) const
			{
				return getValue(a) == getValue(b);
			}
			virtual DescribedBase* getRefValue(const DescribedBase* instance) const
			{
				return getValue(instance);
			}
			virtual void setRefValue(DescribedBase* instance, DescribedBase* value) const
			{
				ReturnType* val = value ? boost::polymorphic_cast<ReturnType*>(value) : NULL;
				setValue(instance, val);
			}
			virtual void readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
			{
				binder.announceIDREF(element, instance, this);
			}
			virtual void writeValue(const DescribedBase* instance, XmlElement* element) const
			{
				element->setValue(InstanceHandle(getValue(instance)));
			}
			virtual void assignIDREF(DescribedBase* propertyOwner, const InstanceHandle& handle) const
			{
				boost::shared_ptr<Instance> t = handle.getTarget();
				setValue(propertyOwner, static_cast<ReturnType*>(t.get()));
			}

		public:
			template<typename GetFunction, typename SetFunction>
			RefPropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags)
				: RefPropertyDescriptor(Class::classDescriptor(), RefType::singleton<Class*>(), name, category, flags),
				  IIDREF(),
				  getset(PropDescriptor<Class, ReturnType*>::getset(get, set))
			{
			}
		};

		// EnumPropDescriptor
		template<typename Class, typename Enum>
		class EnumPropDescriptor : public EnumPropertyDescriptor
		{
		private:
			std::auto_ptr<typename TypedPropertyDescriptor<Enum>::GetSet> getset;
		  
		public:
			template<typename GetFunction, typename SetFunction>
			EnumPropDescriptor(char const* name, char const* category, typename GetFunction get, typename SetFunction set, Functionality flags)
				: EnumPropertyDescriptor(Class::classDescriptor(), EnumDesc<Enum>::singleton(), name, category, flags),
				  getset(PropDescriptor<Class, Enum>::getset<GetFunction, SetFunction>(get, set))
			{
			}
		public:
			virtual bool isReadOnly() const
			{
				return getset->isReadOnly();
			}
			Enum getValue(const DescribedBase* object) const
			{
				return getset->getValue(object);
			}
			void setValue(DescribedBase* object, Enum value) const
			{
				getset->setValue(object, value);
			}
			virtual bool equalValues(const DescribedBase* a, const DescribedBase* b) const
			{
				return getValue(a) == getValue(b);
			}
			virtual int getEnumValue(const DescribedBase* instance) const
			{
				return getValue(instance);
			}
			virtual bool setEnumValue(DescribedBase* instance, int intValue) const
			{
				if (EnumDesc<Enum>::singleton().isValue(intValue))
				{
					setValue(instance, (Enum) intValue);
					return true;
				}

				return false;
			}
			virtual size_t getIndexValue(const DescribedBase* instance) const
			{
				return EnumDesc<Enum>::singleton().convertToIndex(getValue(instance));
			}
			virtual bool setIndexValue(DescribedBase* instance, unsigned index) const
			{
				Enum val;
				if (EnumDesc<Enum>::singleton().convertToValue(index, val))
				{
					setValue(instance, val);
					return true;
				}
				
				return false;
			}
			virtual bool hasStringValue() const;
			virtual std::string getStringValue(const DescribedBase* instance) const
			{
				return EnumDesc<Enum>::singleton().convertToString(getValue(instance));
			}
			virtual bool setStringValue(DescribedBase* instance, const Name& name) const
			{
				Enum val;
				if (EnumDesc<Enum>::singleton().convertToValue(name, val))
				{
					setValue(instance, val);
					return true;
				}

				return false;
			}
			virtual bool setStringValue(DescribedBase* instance, const std::string& text) const
			{
				Enum val;
				if (EnumDesc<Enum>::singleton().convertToValue(text, val))
				{
					setValue(instance, val);
					return true;
				}

				return false;
			}
			virtual void readValue(DescribedBase* instance, const XmlElement* element, IReferenceBinder& binder) const
			{
				if (!element->isXsiNil())
				{
					if (element->isValueType<std::string>())
					{
						std::string text;
						if (element->getValue(text))
						{
							Enum val;
							if (EnumDesc<Enum>::singleton().convertToValue(text, val))
							{
								setValue(instance, val);
								return;
							}

							if (text.size() == 0)
							{
								if (setIndexValue(instance, 0))
								{
									return;
								}
							}
						}
					}

					int value;
					if (element->getValue(value))
					{
						setValue(instance, (Enum) value);
					}
					else
					{
						RBXASSERT(false);
					}
				}
			}
			virtual void writeValue(const DescribedBase* instance, XmlElement* element) const
			{
				element->setValue(getValue(instance));
			}
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
			typedef typename boost::function_traits<Function>::result_type result_type;

		public:
			typedef typename result_type (Class::*FunctionSig)();

		private:
			typename FunctionSig function;
  
		private:
			void declareSignature()
			{
				signature.resultType = &Type::singleton<typename result_type>();
			}

		public:
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
				call<typename result_type>(boost::polymorphic_cast<Class*>(instance), arguments.returnValue);
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
		};

		// One argument
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 1> : public FuncDesc<Class>
		{
		private:
			typedef typename boost::function_traits<Function>::result_type result_type;
			typedef typename boost::function_traits<Function>::arg1_type Arg1;

		public:
			typedef typename result_type (Class::*FunctionSig)(typename Arg1);

		private:
			typename FunctionSig function;
			Value default1;
  
		private:
			void declareSignature(const char* arg1Name)
			{
				signature.resultType = &Type::singleton<typename result_type>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Arg1>(), default1);
			}

		public:
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				Security security)
				: FuncDesc(name, security),
				  function(function)
			{
				declareSignature(arg1Name);
			}
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Arg1 default1,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1(default1)
			{
				declareSignature(arg1Name);
			}
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);

				call<typename result_type>(boost::polymorphic_cast<Class*>(instance), arguments.returnValue, arg1);
			}

		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Arg1>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1) const
			{
				(o->*function)(arg1.convert<typename Arg1>());
			}
		};

		// Two arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 2> : public FuncDesc<Class>
		{
		private:
			typedef typename boost::function_traits<Function>::result_type result_type;
			typedef typename boost::function_traits<Function>::arg1_type Arg1;
			typedef typename boost::function_traits<Function>::arg2_type Arg2;

		public:
			typedef typename result_type (Class::*FunctionSig)(typename Arg1, typename Arg2);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name)
			{
				signature.resultType = &Type::singleton<typename result_type>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Arg1>(), default1);
				signature.addArgument(Name::declare(arg2Name, -1), Type::singleton<typename Arg2>(), default2);
			}

		public:
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				Security security)
				: FuncDesc(name, security),
				  function(function)
			{
				declareSignature(arg1Name, arg2Name);
			}
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Arg2 default2,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default2(default2)
			{
				declareSignature(arg1Name, arg2Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Arg1 default1,
				const char* arg2Name,
				typename Arg2 default2,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1(default1),
				  default2(default2)
			{
				declareSignature(arg1Name, arg2Name);
			}
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);
				Value arg2 = default2;
				arguments.get(2, arg2);

				call<typename result_type>(boost::polymorphic_cast<Class*>(instance), arguments.returnValue, arg1, arg2);
			}
		
		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1, Value& arg2) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1, Value& arg2) const
			{
				(o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>());
			}
		};

		// Three arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 3> : public FuncDesc<Class>
		{
		private:
			typedef typename boost::function_traits<Function>::result_type result_type;
			typedef typename boost::function_traits<Function>::arg1_type Arg1;
			typedef typename boost::function_traits<Function>::arg2_type Arg2;
			typedef typename boost::function_traits<Function>::arg3_type Arg3;

		public:
			typedef typename result_type (Class::*FunctionSig)(typename Arg1, typename Arg2, typename Arg3);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
			Value default3;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name, const char* arg3Name)
			{
				signature.resultType = &Type::singleton<typename result_type>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Arg1>(), default1);
				signature.addArgument(Name::declare(arg2Name, -1), Type::singleton<typename Arg2>(), default2);
				signature.addArgument(Name::declare(arg3Name, -1), Type::singleton<typename Arg3>(), default3);
			}

		public:
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				Security security)
				: FuncDesc(name, security),
				  function(function)
			{
				declareSignature(arg1Name, arg2Name, arg3Name);
			}
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				typename Arg3 default3,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default3(default3)
			{
				declareSignature(arg1Name, arg2Name, arg3Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Arg2 default2,
				const char* arg3Name,
				typename Arg3 default3,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default2(default2),
				  default3(default3)
			{
				declareSignature(arg1Name, arg2Name, arg3Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Arg1 default1,
				const char* arg2Name,
				typename Arg2 default2,
				const char* arg3Name,
				typename Arg3 default3,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1(default1),
				  default2(default2),
				  default3(default3)
			{
				declareSignature(arg1Name, arg2Name, arg3Name);
			}
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);
				Value arg2 = default2;
				arguments.get(2, arg2);
				Value arg3 = default3;
				arguments.get(3, arg3);

				call<typename result_type>(boost::polymorphic_cast<Class*>(instance), arguments.returnValue, arg1, arg2, arg3);
			}

		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1, Value& arg2, Value& arg3) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>(), arg3.convert<typename Arg3>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1, Value& arg2, Value& arg3) const
			{
				(o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>(), arg3.convert<typename Arg3>());
			}
		};

		// Four arguments
		template<typename Class, typename Function>
		class BoundFuncDesc<Class, Function, 4> : public FuncDesc<Class>
		{
		private:
			typedef typename boost::function_traits<Function>::result_type result_type;
			typedef typename boost::function_traits<Function>::arg1_type Arg1;
			typedef typename boost::function_traits<Function>::arg2_type Arg2;
			typedef typename boost::function_traits<Function>::arg3_type Arg3;
			typedef typename boost::function_traits<Function>::arg4_type Arg4;

		public:
			typedef typename result_type (Class::*FunctionSig)(typename Arg1, typename Arg2, typename Arg3, typename Arg4);

		private:
			typename FunctionSig function;
			Value default1;
			Value default2;
			Value default3;
			Value default4;
  
		private:
			void declareSignature(const char* arg1Name, const char* arg2Name, const char* arg3Name, const char* arg4Name)
			{
				signature.resultType = &Type::singleton<typename result_type>();
				signature.addArgument(Name::declare(arg1Name, -1), Type::singleton<typename Arg1>(), default1);
				signature.addArgument(Name::declare(arg2Name, -1), Type::singleton<typename Arg2>(), default2);
				signature.addArgument(Name::declare(arg3Name, -1), Type::singleton<typename Arg3>(), default3);
				signature.addArgument(Name::declare(arg4Name, -1), Type::singleton<typename Arg4>(), default4);
			}

		public:
			//BoundFuncDesc(const BoundFuncDesc&);
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				const char* arg4Name,
				Security security)
				: FuncDesc(name, security),
				  function(function)
			{
				declareSignature(arg1Name, arg2Name, arg3Name, arg4Name);
			}
			
			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				const char* arg4Name,
				typename Arg4 default4,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default4(default4)
			{
				declareSignature(arg1Name, arg2Name, arg3Name, arg4Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				const char* arg3Name,
				typename Arg3 default3,
				const char* arg4Name,
				typename Arg4 default4,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default3(default3),
				  default4(default4)
			{
				declareSignature(arg1Name, arg2Name, arg3Name, arg4Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				const char* arg2Name,
				typename Arg2 default2,
				const char* arg3Name,
				typename Arg3 default3,
				const char* arg4Name,
				typename Arg4 default4,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default2(default2),
				  default3(default3),
				  default4(default4)
			{
				declareSignature(arg1Name, arg2Name, arg3Name, arg4Name);
			}

			BoundFuncDesc(
				typename FunctionSig function,
				const char* name,
				const char* arg1Name,
				typename Arg1 default1,
				const char* arg2Name,
				typename Arg2 default2,
				const char* arg3Name,
				typename Arg3 default3,
				const char* arg4Name,
				typename Arg4 default4,
				Security security)
				: FuncDesc(name, security),
				  function(function),
				  default1(default1),
				  default2(default2),
				  default3(default3),
				  default4(default4)
			{
				declareSignature(arg1Name, arg2Name, arg3Name, arg4Name);
			}
		
		public:
			virtual void execute(DescribedBase* instance, Arguments& arguments) const
			{
				Value arg1 = default1;
				arguments.get(1, arg1);
				Value arg2 = default2;
				arguments.get(2, arg2);
				Value arg3 = default3;
				arguments.get(3, arg3);
				Value arg4 = default4;
				arguments.get(4, arg4);

				call<typename result_type>(boost::polymorphic_cast<Class*>(instance), arguments.returnValue, arg1, arg2, arg3, arg4);
			}

		private:
			template<typename ReturnType>
			void call(Class* o, Value& returnValue, Value& arg1, Value& arg2, Value& arg3, Value& arg4) const
			{
				returnValue.set<ReturnType>((o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>(), arg3.convert<typename Arg3>(), arg4.convert<typename Arg4>()));
			}

			template<>
			void call<void>(Class* o, Value& returnValue, Value& arg1, Value& arg2, Value& arg3, Value& arg4) const
			{
				(o->*function)(arg1.convert<typename Arg1>(), arg2.convert<typename Arg2>(), arg3.convert<typename Arg3>(), arg4.convert<typename Arg4>());
			}
		};
	}
}
