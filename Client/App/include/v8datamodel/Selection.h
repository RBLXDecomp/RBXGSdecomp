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

	class __declspec(novtable) ISelectionBase
	{
		friend class Selection;

	private:
		virtual void onSelectionChanged(const SelectionChanged&) = 0;
	};

	extern const char* const sSelection;

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
			AddIterator(Selection* selection)
				: selection(selection)
			{
			}
			AddIterator& operator*() {return *this;}
			AddIterator operator++(int) {return *this;}
			AddIterator& operator++() {return *this;}

			AddIterator& operator=(Instance* instance)
			{
				selection->addToSelection(instance);
				return *this;
			}
		};

		class RemoveIterator : public std::_Outit
		{
		private:
			Selection* selection;

		public:
			RemoveIterator(Selection* selection)
				: selection(selection)
			{
			}
			RemoveIterator& operator*() {return *this;}
			RemoveIterator operator++(int) {return *this;}
			RemoveIterator& operator++() {return *this;}

			RemoveIterator& operator=(Instance* instance)
			{
				selection->removeFromSelection(instance);
				return *this;
			}
		};

		class ToggleIterator : public std::_Outit
		{
		private:
			Selection* selection;

		public:
			ToggleIterator(Selection* selection)
				: selection(selection)
			{
			}
			ToggleIterator& operator*() {return *this;}
			ToggleIterator operator++(int) {return *this;}
			ToggleIterator& operator++() {return *this;}

			ToggleIterator& operator=(Instance* instance)
			{
				selection->toggleSelection(instance);
				return *this;
			}
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
		const CopyOnWrite<std::vector<boost::shared_ptr<Instance>>>& getSelection() const
		{
			return selection;
		}

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

		void addToSelection(const boost::shared_ptr<Instance>& instance)
		{
			addToSelection(instance.get());
		}

		void addToSelection(Instance* instance);
		void toggleSelection(Instance* instance);
		void removeFromSelection(const Instance* instance);

		bool isSelected(const Instance* instance) const
		{
			return std::find(selection->begin(), selection->end(), shared_from(instance)) != selection->end();
		}

		virtual void onEvent(const Instance* source, AncestorChanged event);
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
		FilteredSelection()
		{
			setName("FilteredSelection");
		}

		virtual ~FilteredSelection()
		{
			if (rootSelection)
				rootSelection->removeFilteredSelection(this);
		}

		Selection* getSelection() const;

		const std::vector<Class*>& items() const
		{
			return filteredSelection;
		}

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
		virtual void onAncestorChanged(const AncestorChanged& event)
		{
			Instance::onAncestorChanged(event);

			if (!rootSelection)
			{
				rootSelection = shared_from(ServiceProvider::create<Selection>(event.newParent));
				if (rootSelection)
				{
					for (std::vector<boost::shared_ptr<Instance>>::const_iterator iter = rootSelection->getSelection()->begin(); iter != rootSelection->getSelection()->end(); iter++)
					{
						Instance* c = iter->get();
						if (c)
							filteredSelection.push_back(c);
					}

					rootSelection->addFilteredSelection(this);
				}
			}
		}

		virtual void onSelectionChanged(const SelectionChanged& event)
		{
			Instance* c = event.addedItem.get();
			if (c)
				filteredSelection.push_back(c);

			Instance* r = event.removedItem.get();
			if (r)
			{
				std::vector<Class*>::iterator iter = std::find(filteredSelection.begin(), filteredSelection.end(), r);
				if (iter != filteredSelection.end())
					filteredSelection.erase(iter);
			}
		}
	};
}
