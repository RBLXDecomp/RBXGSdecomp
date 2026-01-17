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
		class GenericSlotWrapper
		{
		public:
			virtual ~GenericSlotWrapper();
		public:
			virtual void execute(const std::vector<boost::any>&) = 0;
		public:
			//GenericSlotWrapper(const GenericSlotWrapper&);
			GenericSlotWrapper();
		public:
			//GenericSlotWrapper& operator=(const GenericSlotWrapper&);
		};

		class SignalSource;
		class SignalDescriptor;
		class SignalInstance : public boost::noncopyable
		{
		private:
			SignalSource* source;
		public:
			const SignalDescriptor& descriptor;
		  
		public:
			//SignalInstance(const SignalInstance&);
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
			//SignalInstance& operator=(const SignalInstance&);
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
		  
		public:
			//SignalDescriptor(const SignalDescriptor&);
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
		public:
			virtual ~SignalDescriptor() {}
		public:
			//SignalDescriptor& operator=(const SignalDescriptor&);
		};

		class Signal
		{
		protected:
			const SignalDescriptor* descriptor;
			DescribedBase* instance;
		  
		public:
			//Signal(const Signal&);
			Signal(const SignalDescriptor&, DescribedBase*);
		public:
			//Signal& operator=(const Signal&); // maybe?
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
				//TSignalInstance(const TSignalInstance&);
				TSignalInstance(SignalSource* source, const SignalDescriptor& descriptor)
					: SignalInstance(source, descriptor),
					  boost::signal<CallbackSignature>()
				{
				}
				virtual ~TSignalInstance() {}
			public:
				//TSignalInstance& operator=(const TSignalInstance&);
			};

		public:
			//TSignalDesc(const TSignalDesc&);
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
			virtual ~TSignalDesc() {}
			//TSignalDesc& operator=(const TSignalDesc&);
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
				//GenericSlotAdapter(const GenericSlotAdapter&);
				GenericSlotAdapter(GenericSlotWrapper*);
			public:
				void operator()();
			public:
				~GenericSlotAdapter();
			public:
				//GenericSlotAdapter& operator=(const GenericSlotAdapter&);
			};

		public:
			//SignalDescImpl(const SignalDescImpl&);
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

		public:
			virtual ~SignalDescImpl() {}
		public:
			//SignalDescImpl& operator=(const SignalDescImpl&);
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
				//GenericSlotAdapter(const GenericSlotAdapter&);
				GenericSlotAdapter(GenericSlotWrapper*);
			public:
				void operator()(typename FunctionTraits<CallbackSignature>::Arg1Type arg1)
				{
					std::vector<boost::any> args(1);
					args[0] = boost::any(arg1);

					wrapper->execute(args);
				}
			public:
				~GenericSlotAdapter();
			public:
				//GenericSlotAdapter& operator=(const GenericSlotAdapter&);
			};

		public:
			//SignalDescImpl(const SignalDescImpl&);
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

		public:
			virtual ~SignalDescImpl() {}
		public:
			//SignalDescImpl& operator=(const SignalDescImpl&);
		};

		template<typename CallbackSignature>
		class SignalDescImpl<2, CallbackSignature> : public TSignalDesc<CallbackSignature>
		{
		protected:
			class GenericSlotAdapter
			{
			private:
				boost::shared_ptr<GenericSlotWrapper> wrapper;
			  
			public:
				//GenericSlotAdapter(const GenericSlotAdapter&);
				GenericSlotAdapter(GenericSlotWrapper*);
			public:
				void operator()(typename FunctionTraits<CallbackSignature>::Arg1Type arg1, typename FunctionTraits<CallbackSignature>::Arg2Type arg2);
			public:
				~GenericSlotAdapter();
			public:
				//GenericSlotAdapter& operator=(const GenericSlotAdapter&);
			};

		public:
			//SignalDescImpl(const SignalDescImpl&);
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

		public:
			virtual ~SignalDescImpl() {}
		public:
			//SignalDescImpl& operator=(const SignalDescImpl&);
		};

		template<typename Class, typename CallbackSignature>
		class SignalDesc : public SignalDescImpl<FunctionTraits<CallbackSignature>::ArgCount, CallbackSignature>
		{
		public:
			//SignalDesc(const SignalDesc&);
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
			virtual ~SignalDesc() {}
		public:
			//SignalDesc& operator=(const SignalDesc&);
		};
	}
}
