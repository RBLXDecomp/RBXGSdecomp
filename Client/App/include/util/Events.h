#pragma once
#include <vector>

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
		void removeIndex(unsigned int);
	};

	template<typename Class, typename Event>
	class Listener
	{
		friend class Notifier<Class, Event>;

	protected:
		virtual void onEvent(const Class*, Event) = 0;
		Listener& operator=(const Listener&);
		virtual ~Listener();

	public:
		//Listener(const Listener&);
		Listener();
	};

	template<typename Class, typename Event>
	class __declspec(novtable) Notifier
	{
	private:
		std::vector<Listener<Class, Event>*> listeners;
		mutable RaiseRange* raiseRange;

	protected:
		//Notifier(const Notifier&);
		Notifier()
			: listeners(),
			  raiseRange(NULL)
		{
		}
		Notifier& operator=(const Notifier&);

	public:
		void addListener(Listener<Class, Event>*) const;
		void removeListener(Listener<Class, Event>*) const;

	protected:
		bool hasListeners() const
		{
			return !listeners.empty();
		}
		// TODO: does not match
		void raise(Event event, Listener<Class, Event>* listener) const
		{
			listener->onEvent((Class*)this, event);
		}
		void raise(Event event) const
		{
			RaiseRange range = {0, listeners.size(), raiseRange};

			raiseRange = &range;

			for(; range.index < range.upper; range.index++)
			{
				raise(event, listeners[range.index]);
			}

			raiseRange = range.previous;
		}
		void raise() const;
		virtual void onAddListener(Listener<Class, Event>*) const
		{
			return;
		}
		virtual void onRemoveListener(Listener<Class, Event>*) const
		{
			return;
		}
	};
}
