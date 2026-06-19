#include "NetworkSettings.h"

namespace RBX
{
	Reflection::BoundProp<bool, 1> prop_LogPackets("LogPackets", "Diagnostics", &NetworkSettings::logPackets, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_PrintInstances("PrintInstances", "Diagnostics", &NetworkSettings::printInstances, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_PrintProperties("PrintProperties", "Diagnostics", &NetworkSettings::printProperties, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_PrintPacketBuffer("PrintPacketBuffer", "Diagnostics", &NetworkSettings::printPacketBuffer, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<bool, 1> prop_PrintPhysicsErrors("PrintPhysicsErrors", "Diagnostics", &NetworkSettings::printPhysicsErrors, Reflection::PropertyDescriptor::STANDARD);

	Reflection::BoundProp<int, 1> prop_MaxSendBPS("MaxSendBPS", "Simulator", &NetworkSettings::maxSendBPS, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<int, 1> prop_MinExtraPing("MinExtraPing", "Simulator", &NetworkSettings::minExtraPing, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<int, 1> prop_ExtraPingVariance("ExtraPingVariance", "Simulator", &NetworkSettings::extraPingVariance, Reflection::PropertyDescriptor::STANDARD);

	Reflection::BoundProp<int, 1> prop_MaxDataModelSendBuffer("MaxDataModelSendBuffer", "Replication", &NetworkSettings::maxDataModelSendBuffer, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> prop_DataPacketSize("DataPacketSize", "Replication", &NetworkSettings::dataPacketSize, Reflection::PropertyDescriptor::STANDARD);
	Reflection::BoundProp<float, 1> prop_SendRate("SendRate", "Replication", &NetworkSettings::sendRate, Reflection::PropertyDescriptor::STANDARD);

	NetworkSettings::NetworkSettings()
		: clientPhysicsSpeed(1.0f),
		  clientPhysicsLifetime(0.5f),
		  dataPacketSize(0.75f),
		  sendRate(40.0f),
		  clientPhysics(false),
		  printPhysicsErrors(false),
		  printInstances(false),
		  printProperties(false),
		  printPacketBuffer(false),
		  logPackets(false),
		  maxDataModelSendBuffer(4),
		  maxSendBPS(0),
		  minExtraPing(0),
		  extraPingVariance(0)
	{
		setName("Network");
	}
}
