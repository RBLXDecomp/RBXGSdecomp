#pragma once
#include <boost/noncopyable.hpp>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <set>
#include "v8xml/XmlElement.h"

class XmlParser : public boost::noncopyable
{
protected:
	std::streambuf* buffer;
	std::stack<XmlElement*> elements;
  
public:
	//XmlParser(const XmlParser&);
protected:
	XmlParser(std::streambuf* buffer);

public:
	virtual std::auto_ptr<XmlElement> parse() = 0;
 
public:
	~XmlParser();

public:
	//XmlParser& operator=(const XmlParser&);
};

class TextXmlParser : public XmlParser
{
public:
	//TextXmlParser(const TextXmlParser&);
	TextXmlParser(std::streambuf* buffer)
		: XmlParser(buffer)
	{
	}

public:
	virtual std::auto_ptr<XmlElement> parse();

private:
	void skipWhitespace();
	std::string readTag();
	std::string readFirstTag();
	std::string readText(bool);
	std::string removeTag(const std::string&, int&);
	std::string findNextToken(const std::string&, int&);
	std::string findText(const std::string&);
	XmlElement* parseAttributes(const std::string&);

public:
	~TextXmlParser();

public:
	//TextXmlParser& operator=(const TextXmlParser&);
};

class BinaryXmlParser : public XmlParser
{
public:
	//BinaryXmlParser(const BinaryXmlParser&);
	BinaryXmlParser(std::streambuf*);

public:
	virtual std::auto_ptr<XmlElement> parse();

public:
	~BinaryXmlParser();

public:
	//BinaryXmlParser& operator=(const BinaryXmlParser&);
};

class XmlWriter : public boost::noncopyable
{
protected:
	std::map<RBX::InstanceHandle, int> handles;
	std::ostream& stream;
  
public:
	///XmlWriter(const XmlWriter&);
protected:
	XmlWriter(std::ostream&);

public:
	virtual void serialize(const XmlElement*) = 0;
	int getHandleIndex(RBX::InstanceHandle);

public:
	~XmlWriter();

public:
	//XmlWriter& operator=(const XmlWriter&);
};

class TextXmlWriter : public XmlWriter
{
public:
	//TextXmlWriter(const TextXmlWriter&);
	TextXmlWriter(std::ostream&);

protected:
	void serialize(const XmlElement*, int);
public:
	virtual void serialize(const XmlElement*);
protected:
	void writeOpenTag(const XmlElement*, int, const XmlAttribute*);
	void writeCloseTag(const XmlElement*, int);
	void writeText(const std::string&, bool);
	void writeText(const char*, bool);
	virtual void serializeNode(const XmlElement*, int);

public:
	~TextXmlWriter();

public:
	  //TextXmlWriter& operator=(const TextXmlWriter&);
  
public:
	static void encodedWrite(std::ostream&, const std::string&);
	static void encodedWrite(std::ostream&, const char*);
};

class TextXmlWriterWithEmbeddedContent : public TextXmlWriter
{
private:
	std::set<RBX::ContentId> embeddedContent;
  
public:
	//TextXmlWriterWithEmbeddedContent(const TextXmlWriterWithEmbeddedContent&);
	TextXmlWriterWithEmbeddedContent(std::ostream&);

protected:
	virtual void serializeNode(const XmlElement*, int);

public:
	~TextXmlWriterWithEmbeddedContent();

public:
	//TextXmlWriterWithEmbeddedContent& operator=(const TextXmlWriterWithEmbeddedContent&);
};

class BinaryXmlWriter : public XmlWriter
{
public:
	//BinaryXmlWriter(const BinaryXmlWriter&);
	BinaryXmlWriter(std::ostream&);

private:
	void serialize(const std::vector<RBX::InstanceHandle>*);
	void serialize(RBX::InstanceHandle);
	void serialize(const RBX::Name&);
	void serialize(const RBX::Name*);
	void serialize(bool);
	void serialize(float);
	void serialize(unsigned);
	void serialize(int);
	void serialize(RBX::ContentId);
	void serialize(const std::string&);
public:
	virtual void serialize(const XmlElement*);
private:
	void serializeAttribute(const XmlAttribute*);
	void serializeNameValuePair(const XmlNameValuePair*);

public:
	~BinaryXmlWriter();

public:
	//BinaryXmlWriter& operator=(const BinaryXmlWriter&);
};
