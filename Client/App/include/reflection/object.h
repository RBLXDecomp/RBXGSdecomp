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
		class ClassDescriptor : public Descriptor,
								public MemberDescriptorContainer<PropertyDescriptor>,
								public MemberDescriptorContainer<SignalDescriptor>,
								public MemberDescriptorContainer<FunctionDescriptor>
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
			static ClassDescriptor& rootDescriptor() // TODO: check
			{
				static ClassDescriptor root;
				return root;
			}
		};

		class __declspec(novtable) DescribedBase : public SignalSource
		{
		public:
			typedef MemberDescriptorContainer<PropertyDescriptor> MDCProperty;
			typedef MemberDescriptorContainer<FunctionDescriptor> MDCFunction;
			typedef MemberDescriptorContainer<SignalDescriptor> MDCSignal;

		protected:
			const ClassDescriptor* descriptor;
		  
		public:
			DescribedBase()
			{
				ClassDescriptor::lockedDown = true;
				descriptor = &ClassDescriptor::rootDescriptor();
			}
		public:
			const ClassDescriptor& getDescriptor() const
			{
				return *descriptor;
			}

			MDCProperty::ConstIterator findProperty(const Name& name) const
			{
				return getDescriptor().MDCProperty::findConstMember(name, this);
			}
			MDCProperty::Iterator findProperty(const Name& name)
			{
				return getDescriptor().MDCProperty::findMember(name, this);
			}
			MDCProperty::Iterator properties_begin()
			{
				return getDescriptor().MDCProperty::members_begin(this);
			}
			MDCProperty::ConstIterator properties_begin() const
			{
				return getDescriptor().MDCProperty::members_begin(this);
			}
			MDCProperty::Iterator properties_end()
			{
				return getDescriptor().MDCProperty::members_end(this);
			}
			MDCProperty::ConstIterator properties_end() const
			{
				return getDescriptor().MDCProperty::members_end(this);
			}

			MDCFunction::ConstIterator findFunction(const Name& name) const
			{
				return getDescriptor().MDCFunction::findConstMember(name, this);
			}
			MDCFunction::ConstIterator functions_begin() const
			{
				return getDescriptor().MDCFunction::members_begin(this);
			}
			MDCFunction::ConstIterator functions_end() const
			{
				return getDescriptor().MDCFunction::members_end(this);
			}

			MDCSignal::ConstIterator findSignal(const Name& name) const
			{
				return getDescriptor().MDCSignal::findConstMember(name, this);
			}
			MDCSignal::Iterator findSignal(const Name& name)
			{
				return getDescriptor().MDCSignal::findMember(name, this);
			}
			MDCSignal::Iterator signals_begin()
			{
				return getDescriptor().MDCSignal::members_begin(this);
			}
			MDCSignal::ConstIterator signals_begin() const
			{
				return getDescriptor().MDCSignal::members_begin(this);
			}
			MDCSignal::Iterator signals_end()
			{
				return getDescriptor().MDCSignal::members_end(this);
			}
			MDCSignal::ConstIterator signals_end() const
			{
				return getDescriptor().MDCSignal::members_end(this);
			}
		  
		public:
			static ClassDescriptor& classDescriptor()
			{
				return ClassDescriptor::rootDescriptor();
			}
		};
	}
}
