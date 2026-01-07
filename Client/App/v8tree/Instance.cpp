#include "v8tree/Instance.h"
#include "reflection/function.h"
#include <algorithm>

namespace RBX
{
	const char* sInstance = "Instance";

	Reflection::BoundProp<bool, 1> Instance::propArchivable("archivable", "Behavior", &Instance::archivable, Reflection::PropertyDescriptor::STANDARD); // L14
	static Reflection::BoundFuncDesc<Instance, boost::shared_ptr<Instance>(std::string, bool), 2> findFirstChild(&Instance::findFirstChildByName2, "FindFirstChild", "name", "recursive", false, Reflection::FunctionDescriptor::AnyCaller); // L15
	static Reflection::BoundFuncDesc<Instance, boost::shared_ptr<Instance>(), 0> func_clone(&Instance::clone, "Clone", Reflection::FunctionDescriptor::AnyCaller); // L16
	static Reflection::BoundFuncDesc<Instance, void(), 0> func_Remove(&Instance::remove, "Remove", Reflection::FunctionDescriptor::AnyCaller); // L17
	static Reflection::BoundFuncDesc<Instance, boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>(), 0> func_childrenOld(&Instance::getChildren2, "children", Reflection::FunctionDescriptor::AnyCaller); // L18
	static Reflection::BoundFuncDesc<Instance, boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>(), 0> func_children(&Instance::getChildren2, "GetChildren", Reflection::FunctionDescriptor::AnyCaller); // L19
	static Reflection::BoundFuncDesc<Instance, bool(boost::shared_ptr<Instance>), 1> func_isDescendentOf(&Instance::isDescendentOf2, "IsDescendentOf", "ancestor", Reflection::FunctionDescriptor::AnyCaller); // L20
	static Reflection::BoundFuncDesc<Instance, bool(boost::shared_ptr<Instance>), 1> func_isAncestorOf(&Instance::isAncestorOf2, "IsAncestorOf", "descendent", Reflection::FunctionDescriptor::AnyCaller); // L21
	static Reflection::PropDescriptor<Instance, std::string> prop_className("className", "Data", &Instance::getClassNameStr, NULL, Reflection::PropertyDescriptor::UI); // L22

	const Reflection::PropDescriptor<Instance, std::string> Instance::desc_Name("Name", "Data", &Instance::getName, &Instance::setName, Reflection::PropertyDescriptor::STANDARD); // L24
	const Reflection::RefPropDescriptor<Instance, Instance> Instance::propParent("Parent", "Data", &Instance::getParent, &Instance::setParent, Reflection::PropertyDescriptor::UI); // L25

	Reflection::SignalDesc<Instance, void(boost::shared_ptr<Instance>)> Instance::event_childAdded("ChildAdded", "child"); // L27
	Reflection::SignalDesc<Instance, void(boost::shared_ptr<Instance>)> Instance::event_childRemoved("ChildRemoved", "child"); // L28
	Reflection::SignalDesc<Instance, void(boost::shared_ptr<Instance>)> Instance::event_descendentAdded("DescendentAdded", "descendent"); // L29
	Reflection::SignalDesc<Instance, void(boost::shared_ptr<Instance>)> Instance::event_descendentRemoving("DescendentRemoving", "descendent"); // L30
	Reflection::SignalDesc<Instance, void(boost::shared_ptr<Instance>)> Instance::event_ancestryChanged("AncestryChanged", "child"); // L31
	Reflection::SignalDesc<Instance, void(const Reflection::PropertyDescriptor*)> Instance::event_propertyChanged("Changed", "property"); // L32

	static Reflection::SignalDesc<Instance, void(bool)> matchTemp("MatchTEMP", "temp"); 

	Instance::Instance(const char* name)
		: assoc(),
		  parent(NULL),
		  children(),
		  name(name),
		  archivable(true),
		  guid()
	{
	}

	Instance::Instance()
		: assoc(),
		  parent(NULL),
		  children(),
		  name("Instance"),
		  archivable(true),
		  guid()
	{
	}

	bool Instance::isAncestorOf(const Instance* descendent) const
	{
		const Instance* p = descendent;

		while(p != NULL)
		{
			p = p->parent;
			if (p == this)
				return true;
		}

		return false;
	}

	bool Instance::isAncestorOf2(boost::shared_ptr<Instance> descendent)
	{
		bool result = false;

		if (descendent.get())
		{
			Instance* p = descendent->parent;
			result = p == this || isAncestorOf(p);
		}

		return result;
	}

	bool Instance::canAddChild(const Instance* instance) const
	{
		if (instance->contains(this) || instance->parent == this)
			return false;

		return askAddChild(instance) || askSetParent(instance);
	}

	size_t Instance::numChildren() const
	{
		if (&(*children))
			return children->size();
		else
			return 0;
	}

	Instance* Instance::getRootAncestor()
	{
		Instance* ancestor = this;
		while (ancestor->parent != NULL)
		{
			ancestor = ancestor->parent;
		}
		return ancestor;
	}

	void Instance::setName(const std::string& value)
	{
		if (name != value)
		{
			name = value;
			raisePropertyChanged(desc_Name);
		}
	}

	void Instance::raisePropertyChanged(const Reflection::PropertyDescriptor& descriptor)
	{
		PropertyChanged event(Reflection::Property(descriptor, this));
		RBXASSERT(event.getDescriptor().isMemberOf(event.getProperty().getInstance()));

		Notifier<Instance, PropertyChanged>::raise(event);

		if (!event_propertyChanged.empty(this))
			event_propertyChanged.fire(this, &descriptor);

		Instance* p = this->parent;
		if (p)
			p->onChildChanged(this, event);
	}

	void Instance::onDescendentAdded(Instance* instance)
	{
		if (Notifier<Instance, DescendentAdded>::hasListeners())
			Notifier<Instance, DescendentAdded>::raise(DescendentAdded(instance, instance->parent));

		event_descendentAdded.fire(this, shared_from(instance));
	}

	void Instance::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		event_descendentRemoving.fire(this, instance);

		if (Notifier<Instance, DescendentRemoving>::hasListeners())
			Notifier<Instance, DescendentRemoving>::raise(DescendentRemoving(instance, shared_from(instance->parent)));
	}

	class RaiseDescendentAdded2
	{
	private:
		const Instance* notifier;
		Listener<Instance, DescendentAdded>* listener;
	  
	public:
		RaiseDescendentAdded2(const Instance* notifier, Listener<Instance, DescendentAdded>* listener)
			: notifier(notifier),
			  listener(listener)
		{
		}
	public:
		void operator()(Instance* instance)
		{
			notifier->Notifier<Instance, DescendentAdded>::raise(DescendentAdded(instance, instance->parent), listener);
			instance->for_eachChild(*this);
		}
	};

	void Instance::onAddListener(Listener<Instance, DescendentAdded>* listener) const
	{
		for_eachChild(RaiseDescendentAdded2(this, listener));
	}

	void Instance::onAddListener(Listener<Instance, ChildAdded>* listener) const
	{
		if (&(*children))
		{
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> c = children.read();
			for (std::vector<boost::shared_ptr<Instance>>::const_iterator iter = c->begin(); iter != c->end(); iter++)
			{
				Notifier<Instance, ChildAdded>::raise(ChildAdded((*iter).get()), listener);
			}
		}
	}

	void Instance::removeAllChildren()
	{
		while (&(*children))
		{
			(*(children->end() - 1))->setParent(NULL);
		}
	}

	void Instance::onChildChanged(Instance* instance, const PropertyChanged& event)
	{
		Instance* p = parent;

		if (p)
			p->onChildChanged(instance, event);
	}

	void Instance::readProperties(const XmlElement* container, IReferenceBinder& binder)
	{
		for (const XmlElement* i = container->firstChild(); i != NULL; i = i->nextSibling())
			readProperty(i, binder);
	}

	bool Instance::isDescendentOf(const Instance* ancestor) const
	{
		Instance* current = parent;

		while (true)
		{
			if (ancestor == current)
				return true;
			if (!current)
				return false;
			current = current->parent;
		}
	};

	void Instance::remove()
	{
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> r = children.read();

		setParent(NULL);

		if (r)
			std::for_each(r->begin(), r->end(), boost::bind(&Instance::remove, this));
	}

	void Instance::readChild(const XmlElement* childElement, IReferenceBinder& binder)
	{
		const XmlAttribute* classAttrib = childElement->findAttribute(tag_class);

		if (classAttrib)
		{
			const Name* className;
			if (classAttrib->getValue(className))
			{
				boost::shared_ptr<Instance> childInstance = createChild(*className);
				if (childInstance)
				{
					childInstance->read(childElement, binder);
					if (childInstance)
						childInstance->setParent(this);
				}
				else
				{
					const XmlAttribute* nameRef = childElement->findAttribute(name_referent);
					if (nameRef)
						binder.announceID(nameRef, NULL);
				}
			}
		}
	}

	void Instance::readChildren(const XmlElement* element, IReferenceBinder& binder)
	{
		if(element)
		{
			for (const XmlElement* current = element->findFirstChildByTag(tag_Item); current != NULL; current = element->findNextChildWithSameTag(current))
			{
				readChild(current, binder);
			}
		}
	}

	void Instance::read(const XmlElement* element, IReferenceBinder& binder)
	{
		const XmlAttribute* nameRef = element->findAttribute(name_referent);
		if (nameRef)
		{
			binder.announceID(nameRef, this);
		}

		if (element->getTag() == tag_Item)
		{
			const XmlElement* properties = element->findFirstChildByTag(tag_Properties);
			if (properties)
			{
				readProperties(properties, binder);
			}
			readChildren(element, binder);
		}
		else
			readProperty(element, binder);
	}

	void Instance::predelete(Instance* instance)
	{
		instance->predelete();
	}

	void Instance::onAncestorChanged(const AncestorChanged& event)
	{
		typedef std::vector<boost::shared_ptr<Instance>>::const_iterator Iterator;

		if (&(*children))
		{
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> r = children.read();

			for (Iterator it = r->begin(); it != r->end(); it++)
			{
				(*it)->onAncestorChanged(event);
			}
		}

		event_ancestryChanged.fire(this, shared_from(event.oldParent));
		Notifier<Instance, AncestorChanged>::raise(event);
	}

}
