#pragma once
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"
#include <G3D/CoordinateFrame.h>

namespace RBX
{
	extern char* sIntValue;
	extern char* sBoolValue;
	extern char* sFloatValue;
	extern char* sColor3Value;
	extern char* sVector3Value;
	extern char* sBrickColorValue;
	extern char* sStringValue;
	extern char* sCFrameValue;

	template<typename ValueType, const char* const* ClassName>
	class Value : public DescribedCreatable<Value<ValueType, ClassName>, Instance, ClassName>
	{
	private:
		ValueType value;
	private:
		static const ValueType defaultValue;
	public:
		static Reflection::BoundProp<ValueType, 1> desc_Value;
		static Reflection::SignalDesc<Value, void(ValueType)> desc_ValueChanged;

	public:
		Value();
		const Value& getValue() const;
		void setValue(const Value&);
	protected:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}
	private:
		void onValueChanged(const Reflection::PropertyDescriptor&)
		{
			desc_ValueChanged.fire(this, value);
		}
	};

	typedef Value<int,			&sIntValue>			IntValue;
	typedef Value<bool,			&sBoolValue>		BoolValue;
	typedef Value<float,		&sFloatValue>		FloatValue;
	typedef Value<G3D::Color3,	&sColor3Value>		Color3Value;
	typedef Value<G3D::Vector3,	&sVector3Value>		Vector3Value;
	typedef Value<BrickColor,	&sBrickColorValue>	BrickColorValue;
	typedef Value<std::string,	&sStringValue>		StringValue;
	typedef Value<G3D::CoordinateFrame, &sCFrameValue> CFrameValue;

	extern char* sObjectValue;
	
	class ObjectValue : public DescribedCreatable<ObjectValue, Instance, &sObjectValue>
	{
	private:
		boost::shared_ptr<Instance> value;
	public:
		static Reflection::RefPropDescriptor<ObjectValue, Instance> desc_Value;
		static Reflection::SignalDesc<ObjectValue, void(boost::shared_ptr<Instance>)> desc_ValueChanged;

	public:
		ObjectValue();
		Instance* getValue() const
		{
			return value.get();
		}
		void setValue(Instance* value)
		{
			if (this->value.get() != value)
			{
				this->value = shared_from(value);
				raisePropertyChanged(desc_Value);
				desc_ValueChanged.fire(this, this->value);
			}
		}
	protected:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}
	private:
		void onValueChanged(const Reflection::PropertyDescriptor&);
	};

	void registerValueClasses();
}
