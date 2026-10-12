#include "v8datamodel/Value.h"

namespace RBX
{
	char* sIntValue = "IntValue";
	char* sBoolValue = "BoolValue";
	char* sFloatValue = "NumberValue";
	char* sColor3Value = "Color3Value";
	char* sVector3Value = "Vector3Value";
	char* sBrickColorValue = "BrickColorValue";
	char* sStringValue = "StringValue";
	char* sCFrameValue = "CFrameValue";
	char* sObjectValue = "ObjectValue";

	Reflection::BoundProp<int, 1> IntValue::desc_Value("Value", "Data", &IntValue::value, &IntValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<IntValue, void(int)> IntValue::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<bool, 1> BoolValue::desc_Value("Value", "Data", &BoolValue::value, &BoolValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<BoolValue, void(bool)> BoolValue::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<float, 1> FloatValue::desc_Value("Value", "Data", &FloatValue::value, &FloatValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<FloatValue, void(float)> FloatValue::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<G3D::Color3, 1> Color3Value::desc_Value("Value", "Data", &Color3Value::value, &Color3Value::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<Color3Value, void(G3D::Color3)> Color3Value::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<G3D::Vector3, 1> Vector3Value::desc_Value("Value", "Data", &Vector3Value::value, &Vector3Value::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<Vector3Value, void(G3D::Vector3)> Vector3Value::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<BrickColor, 1> BrickColorValue::desc_Value("Value", "Data", &BrickColorValue::value, &BrickColorValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<BrickColorValue, void(BrickColor)> BrickColorValue::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<std::string, 1> StringValue::desc_Value("Value", "Data", &StringValue::value, &StringValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<StringValue, void(std::string)> StringValue::desc_ValueChanged("Changed", "value");

	Reflection::BoundProp<G3D::CoordinateFrame, 1> CFrameValue::desc_Value("Value", "Data", &CFrameValue::value, &CFrameValue::onValueChanged, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<CFrameValue, void(G3D::CoordinateFrame)> CFrameValue::desc_ValueChanged("Changed", "value");

	Reflection::RefPropDescriptor<ObjectValue, Instance> ObjectValue::desc_Value("Value", "Data", &ObjectValue::getValue, &ObjectValue::setValue, Reflection::PropertyDescriptor::STANDARD);
	Reflection::SignalDesc<ObjectValue, void(boost::shared_ptr<Instance>)> ObjectValue::desc_ValueChanged("Changed", "value");

	void registerValueClasses()
	{
		IntValue::classDescriptor();
		FloatValue::classDescriptor();
		BoolValue::classDescriptor();
		StringValue::classDescriptor();
		Vector3Value::classDescriptor();
		CFrameValue::classDescriptor();
		Color3Value::classDescriptor();
		BrickColorValue::classDescriptor();
		ObjectValue::classDescriptor();
	}
}
