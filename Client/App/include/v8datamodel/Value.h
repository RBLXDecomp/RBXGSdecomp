#pragma once
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"
#include <G3D/CoordinateFrame.h>

namespace RBX
{
	extern const char* sIntValue;
	extern const char* sBoolValue;
	extern const char* sFloatValue;
	extern const char* sColor3Value;
	extern const char* sVector3Value;
	extern const char* sBrickColorValue;
	extern const char* sStringValue;
	extern const char* sCFrameValue;

	template<typename ValueType, const char** ClassName>
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
		virtual bool askSetParent(const Instance*) const;
	private:
		void onValueChanged(const Reflection::PropertyDescriptor&);
	};

	typedef Value<int,			&sIntValue>			IntValue;
	typedef Value<bool,			&sBoolValue>		BoolValue;
	typedef Value<float,		&sFloatValue>		FloatValue;
	typedef Value<G3D::Color3,	&sColor3Value>		Color3Value;
	typedef Value<G3D::Vector3,	&sVector3Value>		Vector3Value;
	typedef Value<BrickColor,	&sBrickColorValue>	BrickColorValue;
	typedef Value<std::string,	&sStringValue>		StringValue;
	typedef Value<G3D::CoordinateFrame, &sCFrameValue> CFrameValue;

	extern const char* sObjectValue;
	
	class ObjectValue : public DescribedCreatable<ObjectValue, Instance, &sObjectValue>
	{
	private:
		boost::shared_ptr<Instance> value;
	public:
		static Reflection::RefPropDescriptor<ObjectValue, Instance> desc_Value;
		static Reflection::SignalDesc<ObjectValue, void(boost::shared_ptr<Instance>)> desc_ValueChanged;

	public:
		ObjectValue();
		Instance* getValue() const;
		void setValue(Instance*);
	protected:
		virtual bool askSetParent(const Instance*) const;
	private:
		void onValueChanged(const Reflection::PropertyDescriptor&);
	};

	void registerValueClasses();
}
