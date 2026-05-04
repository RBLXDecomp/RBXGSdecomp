#define _CRT_SECURE_NO_DEPRECATE
#include "v8xml/XmlElement.h"
#include "v8xml/XmlSerializer.h"
#include "util/Utilities.h"
#include "util/Debug.h"

const RBX::Name& value_IDREF_null = RBX::Name::declare("null", 1);
const RBX::Name& value_IDREF_nil = RBX::Name::declare("nil", 2);
const RBX::Name& name_xsinil = RBX::Name::declare("xsi:nil", 3);
const RBX::Name& name_xsitype = RBX::Name::declare("xsi:type", 4);
const RBX::Name& tag_xmlnsxsi = RBX::Name::declare("xmlns:xsi", 5);
const RBX::Name& name_root = RBX::Name::declare("root", 6);
const RBX::Name& name_referent = RBX::Name::declare("referent", 7);
const RBX::Name& name_DeleteItem = RBX::Name::declare("DeleteItem", 8);
const RBX::Name& tag_roblox = RBX::Name::declare("roblox", 9);
const RBX::Name& tag_version = RBX::Name::declare("version", 10);
const RBX::Name& tag_External = RBX::Name::declare("External", 11);
const RBX::Name& name_Ref = RBX::Name::declare("Ref", 12);
const RBX::Name& name_token = RBX::Name::declare("token", 13);
const RBX::Name& name_name = RBX::Name::declare("name", 14);
const RBX::Name& tag_Refs = RBX::Name::declare("Refs", 21);
const RBX::Name& tag_X = RBX::Name::declare("X", 23);
const RBX::Name& tag_Y = RBX::Name::declare("Y", 24);
const RBX::Name& tag_Z = RBX::Name::declare("Z", 25);
const RBX::Name& tag_R00 = RBX::Name::declare("R00", 26);
const RBX::Name& tag_R01 = RBX::Name::declare("R01", 27);
const RBX::Name& tag_R02 = RBX::Name::declare("R02", 28);
const RBX::Name& tag_R10 = RBX::Name::declare("R10", 29);
const RBX::Name& tag_R11 = RBX::Name::declare("R11", 30);
const RBX::Name& tag_R12 = RBX::Name::declare("R12", 31);
const RBX::Name& tag_R20 = RBX::Name::declare("R20", 32);
const RBX::Name& tag_R21 = RBX::Name::declare("R21", 33);
const RBX::Name& tag_R22 = RBX::Name::declare("R22", 34);
const RBX::Name& tag_R = RBX::Name::declare("R", 35);
const RBX::Name& tag_G = RBX::Name::declare("G", 36);
const RBX::Name& tag_B = RBX::Name::declare("B", 37);
const RBX::Name& tag_class = RBX::Name::declare("class", 38);
const RBX::Name& tag_Item = RBX::Name::declare("Item", 39);
const RBX::Name& tag_Properties = RBX::Name::declare("Properties", 40);
const RBX::Name& tag_Feature = RBX::Name::declare("Feature", 41);
const RBX::Name& tag_binary = RBX::Name::declare("binary", 44);
const RBX::Name& tag_hash = RBX::Name::declare("hash", 45);
const RBX::Name& tag_null = RBX::Name::lookup("null");
const RBX::Name& tag_mimeType = RBX::Name::declare("mimeType", 48);
const RBX::Name& tag_xsinoNamespaceSchemaLocation = RBX::Name::declare("xsi:noNamespaceSchemaLocation", 49);

void XmlNameValuePair::clearValue() const
{
	switch (valueType)
	{
	case HANDLE:
		delete handleValue;
		break;
	case STRING:
		delete stringValue;
		break;
	case CONTENTID:
		delete contentIdValue;
		break;
	}

	valueType = NONE;
}

bool XmlNameValuePair::getValue(std::string& value) const
{
	if (valueType == STRING)
	{
		value = *stringValue;
		return true;
	}

	return false;
}

bool XmlNameValuePair::getValue(const RBX::Name*& value) const
{
	if (valueType == NAME)
	{
		value = nameValue;
		return true;
	}
	else if (valueType == STRING)
	{
		value = &RBX::Name::declare(stringValue->c_str(), -1);
		clearValue();
		nameValue = value;
		valueType = NAME;
		return true;
	}

	return false;
}

bool XmlNameValuePair::getValue(RBX::ContentId& value) const
{
	if (valueType == CONTENTID)
	{
		value = *contentIdValue;
		return true;
	}
	else if (valueType == STRING)
	{
		value = RBX::ContentId(*stringValue);
		clearValue();
		contentIdValue = new RBX::ContentId(value);
		valueType = CONTENTID;
		return true;
	}

	return false;
}

bool XmlNameValuePair::getValue(int& value) const
{
	if (valueType == INT)
	{
		value = intValue;
		return true;
	}
	else if (valueType == STRING)
	{
		if (RBX::StringConverter<int>::convertToValue(*stringValue, value))
		{
			clearValue();
			intValue = value;
			valueType = INT;
			return true;
		}
	}

	return false;
}

bool XmlNameValuePair::getValue(unsigned& value) const
{
	if (valueType == UINT)
	{
		value = uintValue;
		return true;
	}
	else if (valueType == STRING)
	{
		if (RBX::StringConverter<unsigned>::convertToValue(*stringValue, value))
		{
			clearValue();
			uintValue = value;
			valueType = UINT;
			return true;
		}
	}

	return false;
}

bool XmlNameValuePair::getValue(bool& value) const
{
	if (valueType == BOOL)
	{
		value = boolValue;
		return true;
	}
	else if (valueType == STRING)
	{
		if (RBX::StringConverter<bool>::convertToValue(*stringValue, value))
		{
			clearValue();
			boolValue = value;
			valueType = BOOL;
			return true;
		}
	}

	return false;
}

bool XmlNameValuePair::getValue(float& value) const
{
	if (valueType == FLOAT)
	{
		value = floatValue;
		return true;
	}
	else if (valueType == STRING)
	{
		if (RBX::StringConverter<float>::convertToValue(*stringValue, value))
		{
			clearValue();
			floatValue = value;
			valueType = FLOAT;
			return true;
		}
	}

	return false;
}

bool XmlNameValuePair::getValue(RBX::InstanceHandle& value) const
{
	if (valueType == NAME && *nameValue == value_IDREF_null)
	{
		clearValue();
		handleValue = new RBX::InstanceHandle(NULL);
		valueType = HANDLE;
	}
	else if (valueType == STRING && value_IDREF_null == *stringValue)
	{
		clearValue();
		handleValue = new RBX::InstanceHandle(NULL);
		valueType = HANDLE;
	}
	else if (valueType == STRING && *stringValue == "")
	{
		clearValue();
		handleValue = new RBX::InstanceHandle(NULL);
		valueType = HANDLE;
	}

	if (valueType == HANDLE)
	{
		value = *handleValue;
		return true;
	}

	return false;
}

void XmlNameValuePair::setValue(std::string value)
{
	clearValue();
	stringValue = new std::string(value);
	valueType = STRING;
}

void XmlNameValuePair::setValue(RBX::ContentId contentId)
{
	clearValue();
	contentIdValue = new RBX::ContentId(contentId);
	valueType = CONTENTID;
}

void XmlNameValuePair::setValue(RBX::InstanceHandle handle)
{
	clearValue();
	handleValue = new RBX::InstanceHandle(handle);
	valueType = HANDLE;
}

template<>
bool XmlNameValuePair::isValueType<RBX::ContentId>() const
{
	return valueType == CONTENTID;
}

template<>
bool XmlNameValuePair::isValueType<std::string>() const
{
	return valueType == STRING;
}

bool XmlNameValuePair::isValueEqual(const RBX::Name* value) const
{
	switch (valueType)
	{
	case NAME:
		return value == nameValue;
	case STRING:
		return *value == *stringValue;
	default:
		return false;
	}
}

std::string XmlNameValuePair::toString(XmlWriter* writer) const
{
	switch (valueType)
	{
	case NONE:
		return "";
	case NAME:
		return nameValue->toString();
	case STRING:
		return *stringValue;
	case CONTENTID:
		return contentIdValue->toString();
	case BOOL:
		return RBX::StringConverter<bool>::convertToString(boolValue);
	case INT:
		return RBX::StringConverter<int>::convertToString(intValue);
	case UINT:
		return RBX::StringConverter<unsigned>::convertToString(uintValue);
	case FLOAT:
		return RBX::StringConverter<float>::convertToString(floatValue);
	case HANDLE:
		if (!handleValue->getTarget())
		{
			return value_IDREF_null.name;
		}
		else
		{
			int index = writer->getHandleIndex(*handleValue);

			char buffer[32];
			sprintf(buffer, "RBX%d", index);
			return buffer;
		}
	default:
		RBXASSERT(0);
		return "";
	}
}

void XmlNameValuePair::replaceHandles(const std::map<RBX::Instance*, RBX::InstanceHandle>& isolationMap)
{
	typedef std::map<RBX::Instance*, RBX::InstanceHandle>::const_iterator Iter;

	if (valueType == HANDLE)
	{
		Iter r = isolationMap.find(handleValue->getTarget().get());

		if (r != isolationMap.end())
		{
			*handleValue = r->second;
		}
	}
}

XmlNameValuePair::XmlNameValuePair(const RBX::Name& tag, const char* text)
	: tag(tag),
	  valueType(STRING),
	  stringValue(new std::string(text))
{
}

XmlNameValuePair::XmlNameValuePair(const RBX::Name& tag, const std::string& text)
	: tag(tag),
	  valueType(STRING),
	  stringValue(new std::string(text))
{
}

XmlNameValuePair::XmlNameValuePair(const RBX::Name& tag, RBX::InstanceHandle handle)
	: tag(tag),
	  valueType(HANDLE),
	  handleValue(new RBX::InstanceHandle(handle))
{
}

XmlNameValuePair::~XmlNameValuePair()
{
	clearValue();
}

XmlAttribute::~XmlAttribute()
{
}

XmlAttribute* XmlElement::findAttribute(const RBX::Name& _tag)
{
	XmlAttribute* attribute;
	for (attribute = attributes.firstChild(); attribute != NULL; attribute = attribute->nextSibling())
	{
		if (attribute->getTag() == _tag)
			return attribute;
	}

	return NULL;
}

const XmlAttribute* XmlElement::findAttribute(const RBX::Name& _tag) const
{
	const XmlAttribute* attribute;
	for (attribute = attributes.firstChild(); attribute != NULL; attribute = attribute->nextSibling())
	{
		if (attribute->getTag() == _tag)
			return attribute;
	}

	return NULL;
}

const XmlElement* XmlElement::findFirstChildByTag(const RBX::Name& _tag) const
{
	const XmlElement* child;
	for (child = firstChild(); child != NULL; child = child->nextSibling())
	{
		if (child->getTag() == _tag)
			return child;
	}

	return NULL;
}

const XmlElement* XmlElement::findNextChildWithSameTag(const XmlElement* node) const
{
	const XmlElement* child;
	for (child = node->nextSibling(); child != NULL; child = child->nextSibling())
	{
		if (child->getTag() == node->getTag())
			return child;
	}

	return NULL;
}

bool XmlElement::isXsiNil() const
{
	bool isXsiNil;
	const XmlAttribute* attribute = findAttribute(name_xsinil);
	return attribute && attribute->getValue(isXsiNil) && isXsiNil;
}

XmlElement* XmlElement::addChild(const RBX::Name& _tag)
{
	return addChild(new XmlElement(_tag));
}

XmlElement* XmlElement::addChild(XmlElement* element)
{
	pushBackChild(element);
	return element;
}
