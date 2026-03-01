#include "v8xml/SerializerV2.h"
#include "v8tree/Instance.h"
#include <boost/bind.hpp>
#include <algorithm>

namespace RBX
{
	// NOTE: ALL FUNCTIONS IN MERGEBINDER ARE DEFINED IN THE HEADER

	void MergeBinder::announceID(const XmlNameValuePair* valueID, Instance* target)
	{
		processID(valueID, target);
	}

	void MergeBinder::announceIDREF(const XmlNameValuePair* valueIDREF, Reflection::DescribedBase* propertyOwner, const IIDREF* idref)
	{
		bool success = processIDREF(valueIDREF, propertyOwner, idref);
		RBXASSERT(success);
	}

	bool MergeBinder::processID(const XmlNameValuePair* valueID, Instance* source)
	{
		InstanceHandle h;
		if (valueID->getValue(h))
		{
			h.linkTo(shared_from(source));
			return true;
		}
		else if (valueID->isValueEqual(&value_IDREF_nil))
			return true;
		else
			return false;
	}

	bool MergeBinder::processIDREF(const XmlNameValuePair* valueIDREF, Reflection::DescribedBase* propertyOwner, const IIDREF* idref)
	{
		InstanceHandle h;
		if (valueIDREF->getValue(h))
		{
			if(!h.empty())
				idref->assignIDREF(propertyOwner, h);
			else
			{
				IDREFItem item = {idref, propertyOwner, h};
				deferredIDREFItems.push_back(item);
			}
			return true;
		}
		else if (valueIDREF->isValueEqual(&value_IDREF_nil))
			return true;
		else
			return false;
	}

	bool MergeBinder::resolveRefs()
	{
		for (std::vector<IDREFItem>::iterator iter = deferredIDREFItems.begin(); iter != deferredIDREFItems.end(); iter++)
		{
			const IIDREF*& idref = iter->idref;
			Reflection::DescribedBase*& propertyOwner = iter->propertyOwner;
			InstanceHandle& value = iter->value;

			idref->assignIDREF(propertyOwner, value);
		}

		deferredIDREFItems.clear();

		return true;
	}
}

bool ArchiveBinder::resolveRefs()
{
	size_t counts = std::count_if(idrefBindings.begin(), idrefBindings.end(), boost::bind(&ArchiveBinder::resolveIDREF, this, _1));

	return MergeBinder::resolveRefs() && counts == idrefBindings.size();
}

bool ArchiveBinder::resolveIDREF(IDREFBinding binding)
{
	std::string s;
	bool success = binding.valueIDREF->getValue(s);

	RBXASSERT(success);
	RBXASSERT(value_IDREF_nil.name.compare(s));
	RBXASSERT(value_IDREF_null.name.compare(s));
	RBXASSERT(s != "");

	std::map<std::string, RBX::InstanceHandle>::iterator found = idMap.find(s);
	if(found != idMap.end())
	{
		RBX::InstanceHandle& foundHandle = found->second;
		binding.idref->assignIDREF(binding.propertyOwner,foundHandle);
		return true;
	}
	else
	{
		binding.idref->assignIDREF(binding.propertyOwner,RBX::InstanceHandle(NULL));
		return false;
	}
}

bool ArchiveBinder::processID(const XmlNameValuePair* valueID, RBX::Instance* source)
{
	if (!MergeBinder::processID(valueID, source))
	{
		std::string s;
		bool success = valueID->getValue(s);

		RBXASSERT(success);
		RBXASSERT(idMap.find(s) == idMap.end());

		idMap[s] = shared_from(source);
	}

	return true;
}

bool ArchiveBinder::processIDREF(const XmlNameValuePair* valueIDREF, RBX::Reflection::DescribedBase* propertyOwner, const RBX::IIDREF* idref)
{
	if(!MergeBinder::processIDREF(valueIDREF, propertyOwner, idref))
	{
		IDREFBinding binding = {valueIDREF, propertyOwner, idref};
		idrefBindings.push_back(binding);
	}

	return true;
}

void buildIsolationMap(XmlElement* element, std::map<RBX::Instance*, RBX::InstanceHandle>& isolationMap);

void isolate(XmlElement* element, const std::map<RBX::Instance*, RBX::InstanceHandle>& isolationMap)
{
	element->replaceHandles(isolationMap);

	for(XmlAttribute* attr = element->getFirstAttribute(); attr != NULL; attr = element->getNextAttribute(attr))
		attr->replaceHandles(isolationMap);

	for(XmlElement* elem = element->firstChild(); elem != NULL; elem = element->nextChild(elem))
		isolate(elem, isolationMap);
}

void SerializerV2::isolateHandles(XmlElement* root)
{
	std::map<RBX::Instance*, RBX::InstanceHandle> isolationMap;

	buildIsolationMap(root, isolationMap);
	isolate(root, isolationMap);
}

XmlElement* SerializerV2::newRootElement()
{
	static const RBX::Name& tag_xmlnsxmime = RBX::Name::declare("xmlns:xmime", -1);

	XmlElement* thisElement = new XmlElement(tag_roblox);
	thisElement->addAttribute(tag_xmlnsxmime,"http://www.w3.org/2005/05/xmlmime");
	thisElement->addAttribute(tag_xmlnsxsi,"http://www.w3.org/2001/XMLSchema-instance");
	thisElement->addAttribute(tag_xsinoNamespaceSchemaLocation,"http://www.roblox.com/roblox.xsd");

	thisElement->addAttribute(tag_version, 4);

	thisElement->pushBackChild(new XmlElement(tag_External, &value_IDREF_null));
	thisElement->pushBackChild(new XmlElement(tag_External, &value_IDREF_nil));

	return thisElement;
}
