#pragma once
#include <boost/shared_ptr.hpp>
#include <vector>
#include "reflection/descriptor.h"
#include "reflection/member.h"
#include "reflection/signal.h"
#include "reflection/property.h"
#include "reflection/function.h"
#include "util/Name.h"
#include "util/Object.h"

namespace RBX
{
	namespace Reflection
	{
		class ClassDescriptor : public Descriptor, public MemberDescriptorContainer<PropertyDescriptor>, public MemberDescriptorContainer<SignalDescriptor>, public MemberDescriptorContainer<FunctionDescriptor>
		{
		public:
			typedef MemberDescriptorContainer<PropertyDescriptor> PropertyContainer;
			typedef MemberDescriptorContainer<SignalDescriptor> SignalContainer;
			typedef MemberDescriptorContainer<FunctionDescriptor> FunctionContainer;

		private:
			std::vector<ClassDescriptor*> derivedClasses;
			ClassDescriptor* base;
		public:
			static bool lockedDown;
		  
		public:
			//ClassDescriptor(const ClassDescriptor&);
			ClassDescriptor(ClassDescriptor& base, const char* name);
		private:
			ClassDescriptor();
		public:
			const ClassDescriptor* getBase() const
			{
				return base;
			}
			bool isBaseOf(const char*) const;
			bool isBaseOf(const ClassDescriptor& base) const;
			bool isA(const char*) const;
			bool isA(const ClassDescriptor&) const;
			std::vector<ClassDescriptor*>::const_iterator derivedClasses_begin() const;
			std::vector<ClassDescriptor*>::const_iterator derivedClasses_end() const;
			bool operator==(const ClassDescriptor& other) const;
			bool operator!=(const ClassDescriptor& other) const;
		public:
			virtual ~ClassDescriptor();
		public:
			//ClassDescriptor& operator=(const ClassDescriptor&);
		  
		public:
			static ClassDescriptor& rootDescriptor() // TODO: check
			{
				static ClassDescriptor root;
				return root;
			}
		};

		class DescribedBase : public SignalSource
		{
		public:
			typedef MemberDescriptorContainer<PropertyDescriptor> MDCProperty;
			typedef MemberDescriptorContainer<FunctionDescriptor> MDCFunction;
			typedef MemberDescriptorContainer<SignalDescriptor> MDCSignal;

		protected:
			const ClassDescriptor* descriptor;
		  
		public:
			//DescribedBase(const DescribedBase&);
			DescribedBase() {}
		public:
			const ClassDescriptor& getDescriptor() const
			{
				return *descriptor;
			}
			MDCProperty::ConstIterator findProperty(const Name&) const;
			MDCProperty::Iterator findProperty(const Name&);
			MDCProperty::Iterator properties_begin();
			MDCProperty::ConstIterator properties_begin() const;
			MDCProperty::Iterator properties_end();
			MDCProperty::ConstIterator properties_end() const;
			MDCFunction::ConstIterator findFunction(const Name&) const;
			MDCFunction::ConstIterator functions_begin() const;
			MDCFunction::ConstIterator functions_end() const;
			MDCSignal::ConstIterator findSignal(const Name&) const;
			MDCSignal::Iterator findSignal(const Name&);
			MDCSignal::Iterator signals_begin();
			MDCSignal::ConstIterator signals_begin() const;
			MDCSignal::Iterator signals_end();
			MDCSignal::ConstIterator signals_end() const;
		public:
			virtual ~DescribedBase();
		public:
			//DescribedBase& operator=(const DescribedBase&);
		  
		public:
			static ClassDescriptor& classDescriptor()
			{
				return ClassDescriptor::rootDescriptor();
			}
		};
	}
}
