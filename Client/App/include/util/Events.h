#include <vector>
#include "util/standardout.h"

#ifndef EVENTS_H // unfortunately, this is a hack we have to do because of functions defined in headers
#define EVENTS_H

namespace RBX
{
	template<typename Class, typename Event>
	class Notifier;

	struct RaiseRange
	{
	public:
		size_t index;
		size_t upper;
		RaiseRange* previous;
	public:
		void removeIndex(size_t);
	};

	template<typename Class, typename Event>
	class __declspec(novtable) Listener
	{
		friend class Notifier<Class, Event>;

	protected:
		virtual void onEvent(const Class*, Event) = 0;
		Listener& operator=(const Listener&);

		virtual ~Listener()
		{
		}
	};

	template<typename Class, typename Event>
	class __declspec(novtable) Notifier
	{
	private:
		mutable std::vector<Listener<Class, Event>*> listeners;
		mutable RaiseRange* raiseRange;

	protected:
		Notifier(const Notifier&);

		Notifier()
			: listeners(),
			  raiseRange(NULL)
		{
		}

		Notifier& operator=(const Notifier&);

		virtual ~Notifier()
		{
		}

	public:
		void addListener(Listener<Class, Event>* listener) const
		{
			if (std::find(listeners.begin(), listeners.end(), listener) == listeners.end())
			{
				listeners.push_back(listener);
				onAddListener(listener);
			}
		}
		void removeListener(Listener<Class, Event>* listener) const
		{
			std::vector<Listener<Class, Event>*>::iterator iter = std::find(listeners.begin(), listeners.end(), listener);

			if (iter != listeners.end())
			{
				onRemoveListener(listener);
				if (raiseRange)
				{
					raiseRange->removeIndex(std::distance(listeners.begin(), iter));
				}
				listeners.erase(iter);
			}
		}

	protected:
		bool hasListeners() const
		{
			return !listeners.empty();
		}

		void raise(Event event, Listener<Class, Event>* listener) const
		{
			try
			{
				listener->onEvent((Class*)this, event);
			}
			catch (std::exception& exp)
			{
				std::string what = exp.what();
				StandardOut::singleton()->print(MESSAGE_WARNING, "Exception caught in onEvent. %s", what.c_str());
			}
		}

		void raise(Event event) const
		{
			RaiseRange range = {0, listeners.size(), raiseRange};

			raiseRange = &range;

			for (; range.index < range.upper; range.index++)
			{
				raise(event, listeners[range.index]);
			}

			raiseRange = range.previous;
		}

		void raise() const
		{
			raise(Event());
		}

		virtual void onAddListener(Listener<Class, Event>*) const
		{
			return;
		}

		virtual void onRemoveListener(Listener<Class, Event>*) const
		{
			return;
		}
	public:
		template<typename T>
		static void connect(const T& notifier, Listener<Class, Event>* listener)
		{
			if (notifier)
				notifier->Notifier<Class, Event>::addListener(listener);
		}

		template<typename T>
		static void disconnect(const T& notifier, Listener<Class, Event>* listener)
		{
			if (notifier)
				notifier->Notifier<Class, Event>::removeListener(listener);
		}
	};
}

#endif // EVENTS_H
