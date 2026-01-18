#include "Streaming.h"
#include <BitStream.h>
#include <StringCompressor.h>
#include <algorithm>

namespace RBX
{
	namespace Network
	{
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, bool value)
		{
			stream.Write(value);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, int value)
		{
			stream.Write(value);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, unsigned int value)
		{
			stream.Write(value);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, unsigned char value)
		{
			stream.Write(value);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, float value)
		{
			stream.Write(value);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const std::string& value)
		{
			stream.Write((unsigned)value.size());
			stringCompressor->EncodeString(value.c_str(), (int)value.size() + 1, &stream);
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const G3D::Vector3& value)
		{
			stream << value.x;
			stream << value.y;
			stream << value.z;
			return stream;
		}

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const G3D::Color3& value)
		{
			stream << value.r;
			stream << value.g;
			stream << value.b;
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, bool& value)
		{
			stream.Read(value);
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, int& value)
		{
			stream.Read(value);
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, unsigned int& value)
		{
			stream.Read(value);
			return stream;
		}


		RakNet::BitStream& operator>>(RakNet::BitStream& stream, unsigned char& value)
		{
			stream.Read(value);
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, float& value)
		{
			stream.Read(value);
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, std::string& value)
		{
			unsigned int strSize;
			stream >> strSize;

			char* strBuf = new char[strSize + 1];
			stringCompressor->DecodeString(strBuf, strSize + 1, &stream);
			value = strBuf;

			delete[] strBuf;
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, G3D::Vector3& value)
		{
			stream >> value.x;
			stream >> value.y;
			stream >> value.z;
			return stream;
		}

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, G3D::Color3& value)
		{
			stream >> value.r;
			stream >> value.g;
			stream >> value.b;
			return stream;
		}

		template<typename T>
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, T& value) // TODO: check match
		{
			return operator>>(stream, value);
		}

		bool brickEq(float a, float b)
		{
			return a == b || fabs(a - b) <= 0.0005f;
		}

		static bool isBrickLocation(G3D::Vector3& value, short& x, unsigned short& y, short& z); // TODO: implement

		void deserializeEnum(Reflection::Property& property, RakNet::BitStream& bitStream)
		{
			const Reflection::EnumPropertyDescriptor& prop = static_cast<const Reflection::EnumPropertyDescriptor&>(property.getDescriptor());

			int value = 0;
			bitStream.ReadBits((unsigned char*)&value, (int)prop.enumDescriptor.getEnumCountMSB() + 1);

			RBXASSERT(value >= 0);
			RBXASSERT(value < (int)prop.enumDescriptor.getEnumCount());

			prop.setIndexValue(property.getInstance(), value);
		}

		void serializeEnum(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream)
		{
			const Reflection::EnumPropertyDescriptor& prop = static_cast<const Reflection::EnumPropertyDescriptor&>(property.getDescriptor());

			int value = prop.getIndexValue(property.getInstance());

			RBXASSERT(value >= 0);
			RBXASSERT(value < (int)prop.enumDescriptor.getEnumCount());

			bitStream.WriteBits((unsigned char*)&value, (int)prop.enumDescriptor.getEnumCountMSB() + 1);
		}

		void StringReceiver::receive(RakNet::BitStream& stream, std::string& value)
		{
			unsigned char id;
			stream >> id;

			if (id == 0)
			{
				value.clear();
			}
			else if (id >> 7)
			{
				stream >> value;
				dictionary[id & 127] = value;
			}
			else
			{
				value = dictionary[id];
			}
		}

		void StringReceiver::receive(RakNet::BitStream& stream, const Name*& value)
		{
			std::string s;
			receive(stream, s);

			value = &Name::declare(s.c_str(), -1);
		}

		void StringReceiver::deserializeString(Reflection::Property& property, RakNet::BitStream& bitStream)
		{
			std::string value;
			receive(bitStream, value);
			
			Instance* instance = static_cast<Instance*>(property.getInstance());
			const Reflection::PropertyDescriptor& desc = property.getDescriptor();

			desc.setStringValue(instance, value);
		}

		void StringSender::serializeString(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream)
		{
			const Instance* instance = static_cast<const Instance*>(property.getInstance());
			const Reflection::PropertyDescriptor& desc = property.getDescriptor();

			std::string value = desc.getStringValue(instance);
			send(bitStream, value);
		}

		void IdSerializer::serializeId(RakNet::BitStream& stream, const Instance* instance)
		{
			if (instance)
			{
				Guid::Data id;
				instance->getGuid().extract(id);

				scopeNames.send(stream, id.scope->name);

				RBXASSERT((id.index & 0xff000000) == 0);
				stream.WriteBits((unsigned char*)&id.index, 24);
			}
			else
			{
				scopeNames.send(stream, Name::getNullName().name);
			}
		}

		bool IdSerializer::trySerializeId(RakNet::BitStream& stream, const Instance* instance)
		{
			if (instance)
			{
				Guid::Data id;
				instance->getGuid().extract(id);

				if (!scopeNames.trySend(stream, id.scope->name))
					return false;

				RBXASSERT((id.index & 0xff000000) == 0);
				stream.WriteBits((unsigned char*)&id.index, 24);
				return true;
			}
			else
			{
				scopeNames.send(stream, Name::getNullName().name);
				return true;
			}
		}

		void IdSerializer::resolvePendingBindings(Instance* instance, Guid::Data id)
		{
			std::map<Guid::Data, std::vector<WaitItem>>::iterator iter = waitItems.find(id);
			if (iter != waitItems.end())
			{
				std::for_each(iter->second.begin(), iter->second.end(), boost::bind(&IdSerializer::setRefValue, _1, instance));
				waitItems.erase(iter);
			}
		}

		void IdSerializer::serializeRef(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream)
		{
			const Reflection::RefPropertyDescriptor& prop = static_cast<const Reflection::RefPropertyDescriptor&>(property.getDescriptor());
			serializeId(bitStream, (Instance*)prop.getRefValue(property.getInstance()));
		}

		void IdSerializer::setRefValue(WaitItem& wi, Instance* instance)
		{
			wi.desc->setRefValue(wi.instance.get(), instance);
		}

		void IdSerializer::deserializeId(RakNet::BitStream& stream, Guid::Data& id)
		{
			scopeNames.receive(stream, id.scope);
			if (*id.scope != Name::getNullName())
			{
				id.index = 0;
				stream.ReadBits((unsigned char*)&id.index, 24);
			}
			else
			{
				id.index = 0;
			}
		}
	}
}
