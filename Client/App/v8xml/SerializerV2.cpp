#include "v8xml/SerializerV2.h"
#include "v8datamodel/DataModel.h"
#include <boost/bind.hpp>
#include <G3D/Log.h>
#include <algorithm>

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
	if (found != idMap.end())
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
	if (!MergeBinder::processIDREF(valueIDREF, propertyOwner, idref))
	{
		IDREFBinding binding = {valueIDREF, propertyOwner, idref};
		idrefBindings.push_back(binding);
	}

	return true;
}

static void buildIsolationMap(XmlElement* element, std::map<RBX::Instance*, RBX::InstanceHandle>& isolationMap)
{
	XmlAttribute* referent = element->findAttribute(name_referent);
	if (referent)
	{
		RBX::InstanceHandle h;
		bool success = referent->getValue(h);

		RBXASSERT(success);
		RBXASSERT(isolationMap.find(h.getTarget().get()) == isolationMap.end());

		isolationMap[h.getTarget().get()] = RBX::InstanceHandle();
	}

	for (XmlElement* child = element->firstChild(); child != NULL; child = element->nextChild(child))
	{
		buildIsolationMap(child, isolationMap);
	}
}

static void isolate(XmlElement* element, const std::map<RBX::Instance*, RBX::InstanceHandle>& isolationMap)
{
	element->replaceHandles(isolationMap);

	for (XmlAttribute* attr = element->getFirstAttribute(); attr != NULL; attr = element->getNextAttribute(attr))
	{
		attr->replaceHandles(isolationMap);
	}

	for (XmlElement* elem = element->firstChild(); elem != NULL; elem = element->nextChild(elem))
	{
		isolate(elem, isolationMap);
	}
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

void SerializerV2::load(XmlElement* root, RBX::DataModel* dataModel)
{
	ArchiveBinder binder;
	dataModel->readChildren(root, binder);

	binder.resolveRefs();
}

void SerializerV2::loadXML(std::istream& stream, RBX::DataModel* dataModel)
{
	TextXmlParser machine(stream.rdbuf());
	std::auto_ptr<XmlElement> root = machine.parse();

	if (root->getTag() == tag_roblox)
	{
		XmlAttribute* version = root->findAttribute(tag_version);
		if (!version->getValue(schemaVersionLoading))
			throw std::runtime_error("SerializerV2::loadXML no version number");

		if (schemaVersionLoading < 4)
			throw std::runtime_error("SerializerV2::loadXML schemaVersionLoading<4");

		load(root.get(), dataModel);
		dataModel->setDirty(false);
	}
	else
	{
		schemaVersionLoading = 1;
		throw std::runtime_error("SerializerV2::loadXML ill-formed XML. No Roblox tag");
	}
}

void SerializerV2::loadInstances(XmlElement* root, std::vector<boost::shared_ptr<RBX::Instance>>& result)
{
	if (root->getTag() == tag_roblox)
	{
		XmlAttribute* versionAttribute = root->findAttribute(tag_version);
		if (versionAttribute && versionAttribute->getValue(schemaVersionLoading) && schemaVersionLoading >= 4)
		{
			ArchiveBinder binder;
			const XmlElement* item = root->findFirstChildByTag(tag_Item);
			while (item != NULL)
			{
				const RBX::Name* className = NULL;
				if (item->findAttributeValue(tag_class, className))
				{
					boost::shared_ptr<RBX::Instance> instance = RBX::AbstractFactoryProduct<RBX::Instance>::create(*className);
					if (instance)
					{
						instance->read(item, binder);
						result.push_back(instance);
					}
					else
					{
						RBX::StandardOut::singleton()->print(RBX::MESSAGE_WARNING, "Unknown object class \"%s\" while reading XML", className ? className->c_str() : "");
					}
				}

				item = root->findNextChildWithSameTag(item);
			}

			bool success = binder.resolveRefs();
			RBXASSERT(success);

			return;
		}
	}

	G3D::Log::common()->println("Version 1.0 - 3.0 model files not supported - all future version will be supported");
}
