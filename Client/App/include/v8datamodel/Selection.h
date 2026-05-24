#pragma once
#include "v8tree/Instance.h"
#include "v8tree/Service.h"

namespace RBX
{
	class SelectionChanged
	{
	public:
		const boost::shared_ptr<Instance> addedItem;
		const boost::shared_ptr<Instance> removedItem;

	private:
		SelectionChanged(boost::shared_ptr<Instance>, boost::shared_ptr<Instance>);
	};

	class ISelectionBase
	{
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
		size_t size() const;
		boost::shared_ptr<Instance> front() const;
		boost::shared_ptr<Instance> back() const;
		std::vector<boost::shared_ptr<Instance>>::const_iterator begin() const;
		std::vector<boost::shared_ptr<Instance>>::const_iterator end() const;
		const CopyOnWrite<std::vector<boost::shared_ptr<Instance>>> getSelection() const;
		void setSelection(boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>>);
		void setSelection(Instance*);
		void clearSelection();
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> getSelection2();
		void addToSelection(const boost::shared_ptr<Instance>&);
		void addToSelection(Instance*);
		void toggleSelection(Instance*);
		void removeFromSelection(const Instance*);
		bool isSelected(const Instance*) const;
		virtual void onEvent(const Instance*, AncestorChanged);
		void addFilteredSelection(ISelectionBase*);
		void removeFilteredSelection(ISelectionBase*);
	private:
		void raiseAdded(boost::shared_ptr<Instance>);
		void raiseRemoved(boost::shared_ptr<Instance>);
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
