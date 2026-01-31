#pragma once
#include "reflection/member.h"
#include "reflection/type.h"
#include "util/Utilities.h"

namespace RBX
{
	namespace Reflection
	{
		class Function;
		class FunctionDescriptor : public MemberDescriptor
		{
		public:
			enum Security
			{
				NeedTrustedCaller = 0,
				AnyCaller = 1
			};

		public:
			class Arguments
			{
			public:
				Value returnValue;
		  
			public:
				virtual size_t size() const = 0;
				virtual void get(int, Value&) const = 0;
			};

		public:
			typedef Function Describing;

		public:
			const Security security;

		protected:
			SignatureDescriptor signature;
		  
		protected:
			FunctionDescriptor(ClassDescriptor& classDescriptor, const char* name, Security security);

		public:
			const SignatureDescriptor& getSignature() const;
			virtual void execute(DescribedBase*, Arguments&) const = 0;
		};

		class Function
		{
		protected:
			const FunctionDescriptor* descriptor;
			const DescribedBase* instance;
		  
		public:
			Function(const Function&);
			Function(const FunctionDescriptor&, const DescribedBase*);
			Function& operator=(const Function&);
			const Name& getName() const;
			const FunctionDescriptor* getDescriptor() const;
			void execute(FunctionDescriptor::Arguments&) const;
		};

		template<typename Class>
		class FuncDesc : public FunctionDescriptor
		{
		protected:
			FuncDesc(const char* name, Security security)
				: FunctionDescriptor(Class::classDescriptor(), name, security)
			{
			}
		};
	}
}
