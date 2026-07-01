#include "v8datamodel/Selection.h"

namespace RBX
{
	Selection::Selection()
		: selection(std::vector<boost::shared_ptr<Instance>>())
	{
		setName("Selection");
	}

	Selection::~Selection()
	{
		RBXASSERT(size() == 0);
	}

	void Selection::addFilteredSelection(ISelectionBase* fs)
	{
		filteredSelections.push_back(fs);
	}

	void Selection::removeFilteredSelection(ISelectionBase* fs)
	{
		filteredSelections.erase(std::find(filteredSelections.begin(), filteredSelections.end(), fs));
	}

	void Selection::raiseAdded(boost::shared_ptr<Instance> instance)
	{
		SelectionChanged event(instance, boost::shared_ptr<Instance>());

		for (std::vector<ISelectionBase*>::iterator iter = filteredSelections.begin(); iter != filteredSelections.end(); iter++)
		{
			(*iter)->onSelectionChanged(event);
		}

		Notifier<Selection, SelectionChanged>::raise(event);
	}

	void Selection::raiseRemoved(boost::shared_ptr<Instance> source)
	{
		SelectionChanged event(boost::shared_ptr<Instance>(), source);

		for (std::vector<ISelectionBase*>::iterator iter = filteredSelections.begin(); iter != filteredSelections.end(); iter++)
		{
			(*iter)->onSelectionChanged(event);
		}

		Notifier<Selection, SelectionChanged>::raise(event);
	}

	void Selection::toggleSelection(Instance* instance)
	{
		boost::shared_ptr<Instance> shared = shared_from(instance);
		boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>> sel = selection.write();

		std::vector<boost::shared_ptr<Instance>>::iterator found = std::find(sel->begin(), sel->end(), shared);

		if (found == sel->end())
		{
			sel->push_back(shared);
			instance->Notifier<Instance, AncestorChanged>::addListener(this);
			raiseAdded(shared);
		}
		else
		{
			sel->erase(found);
			instance->Notifier<Instance, AncestorChanged>::removeListener(this);
			raiseRemoved(shared);
		}
	}

	void Selection::addToSelection(Instance* instance)
	{
		boost::shared_ptr<Instance> shared = shared_from(instance);
		RBXASSERT(instance);

		boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>> sel = selection.write();

		std::vector<boost::shared_ptr<Instance>>::iterator found = std::find(sel->begin(), sel->end(), shared);

		if (found == sel->end())
		{
			sel->push_back(shared);
			instance->Notifier<Instance, AncestorChanged>::addListener(this);
			raiseAdded(shared);
		}
	}
	
	void Selection::removeFromSelection(const Instance* instance)
	{
		RBXASSERT(instance);

		boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>> sel = selection.write();

		std::vector<boost::shared_ptr<Instance>>::iterator found = std::find(sel->begin(), sel->end(), shared_from(instance));

		if (found != sel->end())
		{
			boost::shared_ptr<Instance> nonConst = *found;

			sel->erase(found);
			instance->Notifier<Instance, AncestorChanged>::removeListener(this);
			raiseRemoved(nonConst);
		}
	}

	void Selection::clearSelection()
	{
		while (!selection->empty())
		{
			boost::shared_ptr<Instance> instance = selection->back();
			selection.write()->pop_back();

			instance->Notifier<Instance, AncestorChanged>::removeListener(this);
			raiseRemoved(instance);
		}
	}

	void Selection::setSelection(Instance* instance)
	{
		boost::shared_ptr<Instance> inst = shared_from(instance);
		
		while (!selection->empty())
		{
			boost::shared_ptr<Instance> item = selection->back();

			if (item == inst)
			{
				if (selection->size() == 1)
					return;

				boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>> sel = selection.write();
				sel->back() = sel->front();
				sel->front() = item;
			}
			else
			{
				selection.write()->pop_back();
				item->Notifier<Instance, AncestorChanged>::removeListener(this);
				raiseRemoved(item);
			}
		}

		if (inst)
		{
			selection.write()->push_back(inst);
			instance->Notifier<Instance, AncestorChanged>::addListener(this);
			raiseAdded(inst);
		}
	}
}
