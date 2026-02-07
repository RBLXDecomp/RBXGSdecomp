#pragma once
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/signals.hpp>
#include <boost/any.hpp>
#include <map>
#include <vector>
#include "reflection/function.h"
#include "util/Utilities.h"

namespace RBX
{
	namespace Reflection
	{
		typedef std::vector<boost::any> Arguments;

		class GenericSlotWrapper
		{
		public:
			virtual ~GenericSlotWrapper();
			virtual void execute(const std::vector<boost::any>&) = 0;

		public:
			template<typename T>
			static GenericSlotWrapper* create(T);
		};

		template<typename T>
		class TGenericSlotWrapper : public GenericSlotWrapper
		{
		private:
			T slot;
		
		private:
		  	TGenericSlotWrapper(const T&);

		public:
			virtual void execute(const Arguments&);
		};

		class SignalSource;
		class SignalDescriptor;
		class SignalInstance : public boost::noncopyable
		{
		private:
			SignalSource* source;
		public:
			const SignalDescriptor& descriptor;
		  
		protected:
			SignalInstance(SignalSource* source, const SignalDescriptor& descriptor)
				: source(source),
				  descriptor(descriptor)
			{
			}

		public:
			virtual ~SignalInstance();

		public:
			SignalSource* getSource()
			{
				return source;
			}

		public:
			template<typename T>
			boost::signals::connection connectGeneric(T slot, boost::signals::connect_position pos);
		};

		class __declspec(novtable) SignalSource
		{
			friend class SignalDescriptor;

		private:
			boost::scoped_ptr<std::map<const SignalDescriptor*, boost::shared_ptr<SignalInstance>>> signals;
		  
		public:
			virtual ~SignalSource();
		public:
			void disconnect_all_slots();
		};

		class Signal;
		class SignalDescriptor : public MemberDescriptor
		{
		public:
			typedef Signal Describing;

		public:
			void (*signalCreatedHook)(SignalSource*);

		protected:
			SignatureDescriptor signature;
		  
		protected:
			SignalDescriptor(ClassDescriptor& classDescriptor, const char* name);

		protected:
			SignalInstance* findSignalInstance(const SignalSource* source) const;

		private:
			virtual boost::signals::connection connectGeneric(SignalInstance*, GenericSlotWrapper*, boost::signals::connect_position) const = 0;
			virtual SignalInstance* newSignalInstance(SignalSource&) const = 0;

		public:
			boost::shared_ptr<SignalInstance> getSignalInstance(SignalSource& source) const;
			const SignatureDescriptor& getSignature() const;
		};

		class Signal
		{
		protected:
			const SignalDescriptor* descriptor;
			DescribedBase* instance;
		  
		public:
			Signal(const Signal&);
			Signal(const SignalDescriptor&, DescribedBase*);

		public:
			Signal& operator=(const Signal&);
			const Name& getName() const;
			const SignalDescriptor* getDescriptor() const;
			boost::shared_ptr<SignalInstance> getSignalInstance() const;
		};

		template<typename CallbackSignature>
		class TSignalDesc : public SignalDescriptor
		{
		protected:
			class TSignalInstance : public SignalInstance, public boost::signal<CallbackSignature>
			{
			public:
				TSignalInstance(SignalSource* source, const SignalDescriptor& descriptor)
					: SignalInstance(source, descriptor),
					  boost::signal<CallbackSignature>()
				{
				}
			};

		protected:
			TSignalDesc(ClassDescriptor& classDescriptor, const char* name)
				: SignalDescriptor(classDescriptor, name)
			{
			}

		private:
			virtual SignalInstance* newSignalInstance(SignalSource& source) const
			{
				return new TSignalInstance(&source, *this);
			}

		protected:
			TSignalInstance* findSig(const SignalSource* source) const
			{
				return (TSignalInstance*)findSignalInstance(source);
			}
			TSignalInstance& sig(SignalSource& source) const // TODO: check match
			{
				return *(TSignalInstance*)(getSignalInstance(source).get());
			}

		public:
			boost::signals::connection connect(SignalSource* source, const boost::slot<boost::function<CallbackSignature>>& slot)
			{
				if (source)
					return sig(*source).connect(slot);
				
				return boost::signals::connection();
			}
			bool empty(const SignalSource* source)
			{
				TSignalInstance* instance = findSig(source);
				return !instance || instance->empty();
			}
		};


		template<int ArgCount, typename CallbackSignature>
		class SignalDescImpl : public TSignalDesc<CallbackSignature>
		{
		};

		// Specialised implementations for different argument counts. Keeping it up to two since Roblox signals ever only had up to two arguments in 2007.
		template<typename CallbackSignature>
		class SignalDescImpl<0, CallbackSignature> : public TSignalDesc<CallbackSignature>
		{
		protected:
			class GenericSlotAdapter
			{
			private:
				boost::shared_ptr<GenericSlotWrapper> wrapper;
			  
			public:
				GenericSlotAdapter(GenericSlotWrapper*);

			public:
				void operator()();
			};

		protected:
			SignalDescImpl(ClassDescriptor& classDescriptor, const char* name)
				: TSignalDesc(classDescriptor, name)
			{
			}

		public:
			void fire(SignalSource* source)
			{
				TSignalInstance* instance = findSig(source);
				if (instance)
					(*instance)();
			}

		protected:
			virtual boost::signals::connection connectGeneric(SignalInstance*, GenericSlotWrapper*, boost::signals::connect_position) const;
		};

		template<typename CallbackSignature>
		class SignalDescImpl<1, CallbackSignature> : public TSignalDesc<CallbackSignature>
		{
		public:
			class GenericSlotAdapter
			{
			private:
				boost::shared_ptr<GenericSlotWrapper> wrapper;
			  
			public:
				GenericSlotAdapter(GenericSlotWrapper*);

			public:
				void operator()(typename FunctionTraits<CallbackSignature>::Arg1Type arg1)
				{
					std::vector<boost::any> args(1);
					args[0] = boost::any(arg1);

					wrapper->execute(args);
				}
			};

		protected:
			SignalDescImpl(ClassDescriptor& classDescriptor, const char* name)
				: TSignalDesc(classDescriptor, name)
			{
			}

		public:
			void fire(SignalSource* source, typename const FunctionTraits<CallbackSignature>::Arg1Type arg1)
			{
				TSignalInstance* instance = findSig(source);
				if (instance)
					(*instance)(arg1);
			}

		protected:
			virtual boost::signals::connection connectGeneric(SignalInstance*, GenericSlotWrapper*, boost::signals::connect_position) const;
		};

		template<typename CallbackSignature>
		class SignalDescImpl<2, CallbackSignature> : public TSignalDesc<CallbackSignature>
		{
		public:
			class GenericSlotAdapter
			{
			private:
				boost::shared_ptr<GenericSlotWrapper> wrapper;
			  
			public:
				GenericSlotAdapter(GenericSlotWrapper*);

			public:
				void operator()(typename FunctionTraits<CallbackSignature>::Arg1Type arg1, typename FunctionTraits<CallbackSignature>::Arg2Type arg2);
			};

		protected:
			SignalDescImpl(ClassDescriptor& classDescriptor, const char* name)
				: TSignalDesc(classDescriptor, name)
			{
			}

		public:
			void fire(SignalSource* source, typename FunctionTraits<CallbackSignature>::Arg1Type arg1, typename FunctionTraits<CallbackSignature>::Arg2Type arg2)
			{
				TSignalInstance* instance = findSig(source);
				if (instance)
					(*instance)(arg1, arg2);
			}

		protected:
			virtual boost::signals::connection connectGeneric(SignalInstance*, GenericSlotWrapper*, boost::signals::connect_position) const;
		};

		template<typename Class, typename CallbackSignature>
		class SignalDesc : public SignalDescImpl<FunctionTraits<CallbackSignature>::ArgCount, CallbackSignature>
		{
		public:
			SignalDesc(const char* name, const char* arg1name, const char* arg2name)
				: SignalDescImpl(Class::classDescriptor(), name)
			{
				SignatureDescriptor::Item arg1(Name::declare(arg1name, -1), Type::singleton<FunctionTraits<typename CallbackSignature>::Arg1Type>());
				signature.arguments.push_back(arg1);

				SignatureDescriptor::Item arg2(Name::declare(arg2name, -1), Type::singleton<FunctionTraits<typename CallbackSignature>::Arg2Type>());
				signature.arguments.push_back(arg2);
			}
			SignalDesc(const char* name, const char* arg1name)
				: SignalDescImpl(Class::classDescriptor(), name)
			{
				SignatureDescriptor::Item arg1(Name::declare(arg1name, -1), Type::singleton<FunctionTraits<typename CallbackSignature>::Arg1Type>());
				signature.arguments.push_back(arg1);
			}
			SignalDesc(const char* name)
				: SignalDescImpl(Class::classDescriptor(), name)
			{
			}
		};
	}
}
