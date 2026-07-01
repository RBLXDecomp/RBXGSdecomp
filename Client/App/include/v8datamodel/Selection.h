#pragma once
#include "v8tree/Instance.h"
#include "v8tree/Service.h"

namespace RBX
{
	class SelectionChanged
	{
		friend class Selection;

	public:
		const boost::shared_ptr<Instance> addedItem;
		const boost::shared_ptr<Instance> removedItem;

	private:
		SelectionChanged(boost::shared_ptr<Instance> added, boost::shared_ptr<Instance> removed)
			: addedItem(added),
			  removedItem(removed)
		{
		}
	};

	class ISelectionBase
	{
		friend class Selection;

	private:
		virtual void onSelectionChanged(const SelectionChanged&) = 0;
	};

	extern const char* sSelection;

	class Selection : public DescribedCreatable<Selection, Instance, &sSelection>,
					  public Notifier<Selection, SelectionChanged>,
					  public Service,
					  public Listener<Instance, AncestorChanged>
	{
		class AddIterator : public std::_Outit // base class has zero typedefs...
		{
		private:
			Selection* selection;

		public:
			AddIterator(Selection*);
			AddIterator& operator*();
			AddIterator operator++(int);
			AddIterator& operator++();
		};

		class RemoveIterator : public std::_Outit
		{
		private:
			Selection* selection;

		public:
			RemoveIterator(Selection*);
			RemoveIterator& operator*();
			RemoveIterator operator++(int);
			RemoveIterator& operator++();
		};

		class ToggleIterator : public std::_Outit
		{
		private:
			Selection* selection;

		public:
			ToggleIterator(Selection*);
			ToggleIterator& operator*();
			ToggleIterator operator++(int);
			ToggleIterator& operator++();
		};

	private:
		CopyOnWrite<std::vector<boost::shared_ptr<Instance>>> selection;
		std::vector<ISelectionBase*> filteredSelections;

	public:
		Selection();
		virtual ~Selection();

		size_t size() const
		{
			return selection->size();
		}

		boost::shared_ptr<Instance> front() const;
		boost::shared_ptr<Instance> back() const;
		std::vector<boost::shared_ptr<Instance>>::const_iterator begin() const;
		std::vector<boost::shared_ptr<Instance>>::const_iterator end() const;
		const CopyOnWrite<std::vector<boost::shared_ptr<Instance>>> getSelection() const;

		template<class Iterator>
		void setSelection(Iterator _First, Iterator _Last)
		{
			clearSelection();

			for (; _First != _Last; _First++)
			{
				addToSelection(*_First);
			}
		}

		void setSelection(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> selection)
		{
			setSelection(selection->begin(), selection->end());
		}

		void setSelection(Instance* instance);
		void clearSelection();

		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> getSelection2()
		{
			return selection.read();
		}

		// might not be right - see Selection::setSelection template overload
		void addToSelection(const boost::shared_ptr<Instance>& instance)
		{
			addToSelection(instance.get());
		}

		void addToSelection(Instance* instance);
		void toggleSelection(Instance* instance);
		void removeFromSelection(const Instance* instance);
		bool isSelected(const Instance*) const;
		virtual void onEvent(const Instance*, AncestorChanged);
		void addFilteredSelection(ISelectionBase* fs);
		void removeFilteredSelection(ISelectionBase* fs);
	private:
		void raiseAdded(boost::shared_ptr<Instance> instance);
		void raiseRemoved(boost::shared_ptr<Instance> source);
	};

	template<typename Class>
	class FilteredSelection : public NonFactoryProduct<Instance, NULL>, public Service, public ISelectionBase
	{
	private:
		boost::shared_ptr<Selection> rootSelection;
		std::vector<Class*> filteredSelection;

	public:
		FilteredSelection();
		virtual ~FilteredSelection();
		Selection* getSelection() const;
		const std::vector<Class*>& items() const;
		size_t size() const;
		const Class* front() const;
		Class* front();
		const Class* back() const;
		Class* back();
		typename std::vector<Class*>::const_iterator begin() const;
		typename std::vector<Class*>::const_iterator end() const;
		void clearSelection();
		void addToSelection(Class*);
		void setSelection(Class*);
	protected:
		virtual void onAncestorChanged(const AncestorChanged&);
		virtual void onSelectionChanged(const SelectionChanged&);
	};
}
