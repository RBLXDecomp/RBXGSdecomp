#pragma once
#include "v8xml/XmlSerializer.h"
#include "v8tree/Instance.h"

namespace RBX
{
	class DataModel;

	// NOTE: may not be intended for this file
	class IReferenceBinder
	{
	public:
		virtual void announceID(const XmlNameValuePair* valueID, Instance* target) = 0;
		virtual void announceIDREF(const XmlNameValuePair* valueIDREF, Reflection::DescribedBase* propertyOwner, const IIDREF* idref) = 0;
	protected:
		void assign(const IIDREF*, Reflection::DescribedBase*, const InstanceHandle&);
	};

	class MergeBinder : public IReferenceBinder
	{
	private:
		struct IDREFItem
		{
			const IIDREF* idref;
			Reflection::DescribedBase* propertyOwner;
			InstanceHandle value;
		};

	private:
		std::vector<IDREFItem> deferredIDREFItems;

	public:
		virtual void announceID(const XmlNameValuePair* valueID, Instance* target)
		{
			processID(valueID, target);
		}
		virtual void announceIDREF(const XmlNameValuePair* valueIDREF, Reflection::DescribedBase* propertyOwner, const IIDREF* idref)
		{
			bool success = processIDREF(valueIDREF, propertyOwner, idref);
			RBXASSERT(success);
		}
		virtual bool resolveRefs()
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
	protected:
		virtual bool processID(const XmlNameValuePair* valueID, Instance* source)
		{
			InstanceHandle h;
			if (valueID->getValue(h))
			{
				h.linkTo(shared_from(source));
				return true;
			}
			else if (valueID->isValueEqual(&value_IDREF_nil))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		virtual bool processIDREF(const XmlNameValuePair* valueIDREF, Reflection::DescribedBase* propertyOwner, const IIDREF* idref)
		{
			InstanceHandle h;
			if (valueIDREF->getValue(h))
			{
				if (!h.empty())
				{
					idref->assignIDREF(propertyOwner, h);
				}
				else
				{
					IDREFItem item = {idref, propertyOwner, h};
					deferredIDREFItems.push_back(item);
				}
				return true;
			}
			else if (valueIDREF->isValueEqual(&value_IDREF_nil))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}

class ArchiveBinder : public RBX::MergeBinder
{
private:
	struct IDREFBinding
	{
		const XmlNameValuePair* valueIDREF;
		RBX::Reflection::DescribedBase* propertyOwner;
		const RBX::IIDREF* idref;
	};

private:
	std::map<std::string, RBX::InstanceHandle> idMap;
	std::list<IDREFBinding> idrefBindings;

public:
	virtual bool processID(const XmlNameValuePair* valueID, RBX::Instance* source);
	virtual bool processIDREF(const XmlNameValuePair* valueIDREF, RBX::Reflection::DescribedBase* propertyOwner, const RBX::IIDREF* idref);
	bool resolveIDREF(IDREFBinding binding);
	virtual bool resolveRefs();
};

class SerializerV2
{
protected:
	int schemaVersionLoading;
public:
	static const int CURRENT_SCHEMA_VERSION;
  
public:
	void loadInstances(XmlElement* root, std::vector<boost::shared_ptr<RBX::Instance>>& result);
	void load(std::istream&, RBX::DataModel*);
	void loadXML(std::istream& stream, RBX::DataModel* dataModel);
	void merge(const XmlElement*, RBX::DataModel*);
  
public:
	static XmlElement* newRootElement();
	static void isolateHandles(XmlElement* root);
	static void load(XmlElement* root, RBX::DataModel* dataModel);
};
