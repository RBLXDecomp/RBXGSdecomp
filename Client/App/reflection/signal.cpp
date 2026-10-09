#include "reflection/signal.h"
#include "reflection/object.h"

namespace RBX
{
	namespace Reflection
	{
		SignalDescriptor::SignalDescriptor(ClassDescriptor& classDescriptor, const char* name)
			: MemberDescriptor(classDescriptor, name, "Signals"),
			  signalCreatedHook(NULL),
			  signature()
		{
			classDescriptor.SignalContainer::declare(this);
		}

		SignalInstance* SignalDescriptor::findSignalInstance(const SignalSource* source) const
		{
			if (!source)
				return NULL;

			if (!source->signals)
				return NULL;

			std::map<const SignalDescriptor*, boost::shared_ptr<SignalInstance>>::const_iterator iter = source->signals->find(this);
			if (iter == source->signals->end())
				return NULL;

			return iter->second.get();
		}

		boost::shared_ptr<SignalInstance> SignalDescriptor::getSignalInstance(SignalSource& source) const
		{
			if (!source.signals)
			{
				source.signals.reset(new std::map<const SignalDescriptor*, boost::shared_ptr<SignalInstance>>);
			}
			else
			{
				std::map<const SignalDescriptor*, boost::shared_ptr<SignalInstance>>::iterator iter = source.signals->find(this);
				if (iter != source.signals->end())
					return iter->second;
			}

			boost::shared_ptr<SignalInstance> signalInstance(newSignalInstance(source));
			(*source.signals)[this] = signalInstance;

			if (signalCreatedHook)
				signalCreatedHook(&source);

			return signalInstance;
		}

		SignalInstance::~SignalInstance() {}

		SignalSource::~SignalSource() {}

		void SignalSource::disconnect_all_slots()
		{
			signals.reset();
		}

		boost::shared_ptr<SignalInstance> Signal::getSignalInstance() const
		{
			return descriptor->getSignalInstance(*instance);
		}
	}
}
