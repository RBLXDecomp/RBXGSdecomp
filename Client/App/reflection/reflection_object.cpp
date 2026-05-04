#include "reflection/member.h"
#include "reflection/object.h"

namespace RBX
{
	namespace Reflection
	{
		bool ClassDescriptor::lockedDown = false;

		ClassDescriptor::ClassDescriptor(ClassDescriptor& base, const char* name)
			: Descriptor(name),
			  PropertyContainer(&base),
			  SignalContainer(&base),
			  FunctionContainer(&base),
			  derivedClasses(),
			  base(&base)
		{
			RBXASSERT(!lockedDown);

			{
				boost::recursive_mutex::scoped_lock lock(sync_0());
				base.derivedClasses.push_back(this);
			}
		}

		ClassDescriptor::ClassDescriptor()
			: Descriptor("<<<ROOT>>>"),
			  PropertyContainer(NULL),
			  SignalContainer(NULL),
			  FunctionContainer(NULL),
			  derivedClasses(),
			  base(NULL)
		{
		}

		bool ClassDescriptor::operator==(const ClassDescriptor& other) const
		{
			return this == &other;
		}

		bool ClassDescriptor::operator!=(const ClassDescriptor& other) const
		{
			return this != &other;
		}

		bool MemberDescriptor::isMemberOf(const ClassDescriptor& classDescriptor) const
		{
			const ClassDescriptor* p = &classDescriptor;
			do
			{
				if (p == &owner)
					return true;
				p = p->getBase();
			}
			while (p != &ClassDescriptor::rootDescriptor());

			return false;
		}

		bool MemberDescriptor::isMemberOf(const DescribedBase* instance) const
		{
			return isMemberOf(instance->getDescriptor());
		}
	}
}
