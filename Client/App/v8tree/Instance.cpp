#include "v8tree/Instance.h"
#include "reflection/function.h"
#include "reflection/property.h"
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

	Instance::~Instance()
	{
		RBXASSERT(!parent);
	}

	bool Instance::askAddChild(const Instance* instance) const
	{
		return false;
	}

	bool Instance::askSetParent(const Instance* instance) const
	{
		return false;
	}

	void Instance::setName(const std::string& value)
	{
		if (name != value)
		{
			name = value;
			raisePropertyChanged(desc_Name);
		}
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
		if (children)
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
		while (children)
		{
			children->back()->setParent(NULL);
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

	void Instance::readChild(const XmlElement* childElement, IReferenceBinder& binder)
	{
		const Name* className = NULL;
		const XmlAttribute* classAttrib = childElement->findAttribute(tag_class);

		if (classAttrib)
		{
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
		if (element)
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

	void Instance::predelete()
	{
		RBXASSERT(!parent);

		while (children)
		{
			boost::shared_ptr<Instance> child;

			{
				boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>> c = children.write();

				child = c->back();

				signalDescendentRemoving(child, this, NULL);

				this->onChildRemoving(child.get());
				child->parent = NULL;
				c->pop_back();

				if (c->empty())
					children.reset();
			}

			event_childRemoved.fire(this, child);
			Notifier<Instance, ChildRemoved>::raise(ChildRemoved(child.get()));

			child->onAncestorChanged(AncestorChanged(child.get(), this, NULL));
			child->raisePropertyChanged(propParent);
		}
	}

	void Instance::onAncestorChanged(const AncestorChanged& event)
	{
		typedef std::vector<boost::shared_ptr<Instance>>::const_iterator Iterator;

		if (children)
		{
			boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> r = children.read();

			for (Iterator it = r->begin(); it != r->end(); it++)
			{
				(*it)->onAncestorChanged(event);
			}
		}

		event_ancestryChanged.fire(this, shared_from(event.child));
		Notifier<Instance, AncestorChanged>::raise(event);
	}

	void Instance::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		return;
	}

	bool Instance::contains(const Instance* child) const
	{
		for (; child != NULL; child = child->parent)
		{
			if (child == this)
				return true;
		}

		return false;
	}

	boost::shared_ptr<Instance> Instance::createChild(const Name& className)
	{
		return AbstractFactoryProduct::create(className);
	}

	int Instance::findChildIndex(const Instance* instance) const
	{
		RBXASSERT(children);

		const std::vector<boost::shared_ptr<Instance>>& c = *children;

		std::vector<boost::shared_ptr<Instance>>::const_iterator found = std::find(c.begin(), c.end(), instance->shared_from_this());
		return (int)std::distance(c.begin(), found);
	}

	void Instance::writeChildren(XmlElement* container)
	{
		if (children)
		{
			const std::vector<boost::shared_ptr<Instance>>& c = *children;

			for (std::vector<boost::shared_ptr<Instance>>::const_iterator it = c.begin(); it != c.end(); it++)
			{
				XmlElement* element = (*it)->write();
				if (element)
					container->pushBackChild(element);
			}
		}
	}

	Instance* Instance::findFirstChildByName(const std::string& findName) const
	{
		if (!children)
			return NULL;

		const std::vector<boost::shared_ptr<Instance>>& c = *children;

		for (size_t i = 0; i < c.size(); i++)
		{
			if (c[i]->name == findName)
				return c[i].get();
		}

		return NULL;
	}

	Instance* Instance::findFirstChildByNameRecursive(const std::string& findName) const
	{
		Instance* foundChild = findFirstChildByName(findName);
		if (foundChild)
			return foundChild;

		if (!children)
			return NULL;

		const std::vector<boost::shared_ptr<Instance>>& c = *children;

		for (size_t i = 0; i < c.size(); i++)
		{
			Instance* child = c[i]->findFirstChildByNameRecursive(findName);
			if (child)
				return child;
		}

		return NULL;
	}

	void Instance::signalDescendentAdded(Instance* instance, Instance* beginParent, Instance* oldParent)
	{
		for (Instance* parent = beginParent; parent != NULL; parent = parent->getParent())
		{
			if (parent == oldParent || parent->isAncestorOf(oldParent))
				break;

			parent->onDescendentAdded(instance);
		}

		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> c = instance->children.read();

		if (c)
		{
			for (std::vector<boost::shared_ptr<Instance>>::const_iterator iter = c->begin(); iter != c->end(); iter++)
			{
				signalDescendentAdded((*iter).get(), beginParent, oldParent);
			}
		}
	}

	void Instance::signalDescendentRemoving(const boost::shared_ptr<Instance>& instance, Instance* beginParent, Instance* newParent)
	{
		for (Instance* parent = beginParent; parent != NULL; parent = parent->getParent())
		{
			if (parent == newParent || parent->isAncestorOf(newParent))
				break;

			parent->onDescendentRemoving(instance);
		}

		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> c = instance->children.read();

		if (c)
		{
			for (std::vector<boost::shared_ptr<Instance>>::const_iterator iter = c->begin(); iter != c->end(); iter++)
			{
				signalDescendentRemoving((*iter), beginParent, newParent);
			}
		}
	}

	//96.19% matching.
	void Instance::readProperty(const XmlElement* propertyElement, IReferenceBinder& binder)
	{
		const Name* value = NULL;

		if (const XmlAttribute* attribute = propertyElement->findAttribute(name_name))
		{
			if (attribute->getValue(value))
			{
				Reflection::PropertyIterator iter = findProperty(*value);

				if (iter != properties_end())
				{
					(*iter).read(propertyElement, binder);
				}
			}
		}
	}

	XmlElement* Instance::write()
	{
		if (archivable)
		{
			if (getClassName() != Name::getNullName())
			{
				XmlElement* element = new XmlElement(tag_Item);
				element->addAttribute(tag_class, &getClassName());
				element->addAttribute(name_referent, InstanceHandle(this));

				writeProperties(element->addChild(tag_Properties));
				writeChildren(element);

				return element;
			}
		}

		return new XmlElement(tag_External, InstanceHandle(this));
	}
  
	void Instance::setParent(Instance* newParent)
	{
		if (newParent == parent)
			return;

		boost::shared_ptr<Instance> oldParent = shared_from(parent);
		boost::shared_ptr<Instance> self = this->shared_from_this();

		if (oldParent)
		{
			if (!oldParent->contains(newParent))
				signalDescendentRemoving(self, oldParent.get(), newParent);

			oldParent->onChildRemoving(this);

			boost::shared_ptr<std::vector<boost::shared_ptr<Instance>>>& c = oldParent->children.write();
			c->erase(std::find(c->begin(), c->end(), self));

			if (c->empty())
				oldParent->children.reset();

			parent = NULL;

			event_childRemoved.fire(oldParent.get(), self);
			oldParent->Notifier<Instance, ChildRemoved>::raise(ChildRemoved(this));

			oldParent->onChildRemoved(this);

			if (oldParent->numChildren() == 0)
				oldParent->onLastChildRemoved();
		}

		if (newParent)
		{
			newParent->children.write()->push_back(self);
		}

		parent = newParent;

		if (newParent)
		{
			newParent->onChildAdded(this);

			if (!newParent->contains(oldParent.get()))
				signalDescendentAdded(this, newParent, oldParent.get());

			newParent->Notifier<Instance, ChildAdded>::raise(ChildAdded(this));
			event_childAdded.fire(newParent, self);
		}

		onAncestorChanged(AncestorChanged(this, oldParent.get(), newParent));
		raisePropertyChanged(propParent);
	}
}
