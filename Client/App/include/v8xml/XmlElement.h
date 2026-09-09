#pragma once
#include <map>
#include "util/Name.h"
#include "util/Handle.h"
#include "util/Utilities.h"
#include "util/ContentProvider.h"

extern const RBX::Name& value_IDREF_null;
extern const RBX::Name& value_IDREF_nil;
extern const RBX::Name& name_xsinil;
extern const RBX::Name& name_xsitype;
extern const RBX::Name& tag_xmlnsxsi;
extern const RBX::Name& name_root;
extern const RBX::Name& name_referent;
extern const RBX::Name& name_DeleteItem;
extern const RBX::Name& tag_roblox;
extern const RBX::Name& tag_version;
extern const RBX::Name& tag_External;
extern const RBX::Name& name_Ref;
extern const RBX::Name& name_token;
extern const RBX::Name& name_name;
extern const RBX::Name& tag_Refs;
extern const RBX::Name& tag_X;
extern const RBX::Name& tag_Y;
extern const RBX::Name& tag_Z;
extern const RBX::Name& tag_R00;
extern const RBX::Name& tag_R01;
extern const RBX::Name& tag_R02;
extern const RBX::Name& tag_R10;
extern const RBX::Name& tag_R11;
extern const RBX::Name& tag_R12;
extern const RBX::Name& tag_R20;
extern const RBX::Name& tag_R21;
extern const RBX::Name& tag_R22;
extern const RBX::Name& tag_R;
extern const RBX::Name& tag_G;
extern const RBX::Name& tag_B;
extern const RBX::Name& tag_class;
extern const RBX::Name& tag_Item;
extern const RBX::Name& tag_Properties;
extern const RBX::Name& tag_Feature;
extern const RBX::Name& tag_binary;
extern const RBX::Name& tag_hash;
extern const RBX::Name& tag_null;
extern const RBX::Name& tag_mimeType;
extern const RBX::Name& tag_xsinoNamespaceSchemaLocation;

// Defined in XmlSerializer.h
class XmlWriter;

class XmlNameValuePair
{
public:
	enum ValueType
	{
		NONE = 0,
		NAME = 1,
		STRING = 2,
		CONTENTID = 3,
		BOOL = 4,
		INT = 5,
		UINT = 6,
		FLOAT = 7,
		HANDLE = 8
	};

private:
	const RBX::Name& tag;
	mutable ValueType valueType;
	union
	{
		std::string* stringValue;
		mutable RBX::ContentId* contentIdValue;
		mutable bool boolValue;
		mutable int intValue;
		mutable unsigned uintValue;
		mutable float floatValue;
		mutable const RBX::Name* nameValue;
		mutable RBX::InstanceHandle* handleValue;
	};

private:
	void clearValue() const;

public:
	XmlNameValuePair(const RBX::Name&, RBX::InstanceHandle);
	XmlNameValuePair(const RBX::Name&, float);
	XmlNameValuePair(const RBX::Name&, bool);
	XmlNameValuePair(const RBX::Name& tag, const RBX::Name* name)
		: tag(tag),
		  valueType(NAME),
	      nameValue(name)
	{
	}

	XmlNameValuePair(const RBX::Name&, const unsigned);
	XmlNameValuePair(const RBX::Name& tag, const int value)
		: tag(tag),
		  valueType(INT),
	      intValue(value)
	{
	}

	XmlNameValuePair(const RBX::Name&, RBX::ContentId);
	XmlNameValuePair(const RBX::Name&, const char*);
	XmlNameValuePair(const RBX::Name&, const std::string&);
	XmlNameValuePair(const RBX::Name& tag)
		: tag(tag),
		  valueType(NONE)
	{
	}
public:
	~XmlNameValuePair();

public:
	const RBX::Name& getTag() const
	{
		return tag;
	}

	bool isValueEmpty() const;

	bool isValueEqual(RBX::InstanceHandle) const;
	bool isValueEqual(const RBX::Name*) const;
	bool isValueEqual(bool) const;
	bool isValueEqual(float) const;
	bool isValueEqual(int) const;
	bool isValueEqual(RBX::ContentId) const;
	bool isValueEqual(const std::string&) const;

	std::string toString(XmlWriter*) const;

	ValueType getValueType() const
	{
		return valueType;
	}

	bool getValue(RBX::InstanceHandle&) const;
	bool getValue(const RBX::Name*&) const;
	bool getValue(bool&) const;
	bool getValue(float&) const;
	bool getValue(unsigned&) const;
	bool getValue(int&) const;
	bool getValue(RBX::ContentId&) const;
	bool getValue(std::string&) const;

	void setValue(RBX::InstanceHandle);
	void setValue(const RBX::Name*);
	void setValue(float);
	void setValue(bool);
	void setValue(unsigned);
	void setValue(int);
	void setValue(const char*);
	void setValue(RBX::ContentId);
	void setValue(std::string);

	void replaceHandles(const std::map<RBX::Instance*, RBX::InstanceHandle>&);

public:
	template<typename Type>
	bool isValueType() const;
};

class XmlAttribute : public RBX::Sibling<XmlAttribute>, public XmlNameValuePair
{
public:
	XmlAttribute(const RBX::Name&);

	template<typename T>
	XmlAttribute(const RBX::Name& tag, T value)
		: XmlNameValuePair(tag, value)
	{
	}

	~XmlAttribute();
};

class XmlElement : public RBX::Sibling<XmlElement>, public RBX::Parent<XmlElement>, public XmlNameValuePair
{
private:
	RBX::Parent<XmlAttribute> attributes;

public:
	XmlElement(const RBX::Name& tag)
		: XmlNameValuePair(tag)
	{
	}

	template<typename T>
	XmlElement(const RBX::Name& tag, T value)
		: XmlNameValuePair(tag, value)
	{
	}

	~XmlElement()
	{
		XmlAttribute* attribute = attributes.firstChild();
		while (attribute)
		{
			XmlAttribute* toBeDestroyed = attribute;
			attribute = attribute->nextSibling();
			delete toBeDestroyed;
		}

		XmlElement* child = firstChild();
		while (child)
		{
			XmlElement* toBeDestroyed = child;
			child = child->nextSibling();
			delete toBeDestroyed;
		}
	}

public:
	bool isXsiNil() const;

protected:
	void addAttribute(XmlAttribute* attribute)
	{
		attributes.pushBackChild(attribute);
	}
public:
	template<typename T>
	void addAttribute(const RBX::Name& _tag, T value)
	{
		addAttribute(new XmlAttribute(_tag, value));
	}

	XmlAttribute* getFirstAttribute()
	{
		return attributes.firstChild();
	}
	const XmlAttribute* getFirstAttribute() const
	{
		return attributes.firstChild();
	}
	XmlAttribute* getNextAttribute(XmlAttribute* attribute)
	{
		return attribute->nextSibling();
	}
	const XmlAttribute* getNextAttribute(const XmlAttribute* attribute) const
	{
		return attribute->nextSibling();
	}
	XmlAttribute* findAttribute(const RBX::Name& _tag);
	const XmlAttribute* findAttribute(const RBX::Name& _tag) const;
	bool findAttributeValue(const RBX::Name&, std::string&) const;
	bool findAttributeValue(const RBX::Name& _tag, const RBX::Name*& _value) const
	{
		const XmlAttribute* attribute = findAttribute(_tag);
		if (!attribute)
			return false;

		return attribute->getValue(_value);
	}

public:
	XmlElement* addChild(const RBX::Name& _tag)
	{
		return addChild(new XmlElement(_tag));
	}
	XmlElement* addChild(XmlElement* element)
	{
		pushBackChild(element);
		return element;
	}
	const XmlElement* findFirstChildByTag(const RBX::Name&) const;
	const XmlElement* findNextChildWithSameTag(const XmlElement*) const;
};
