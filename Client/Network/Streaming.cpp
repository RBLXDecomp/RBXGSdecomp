#include <BitStream.h>
#include <StringCompressor.h>
#include "Streaming.h"
#include "IdManager.h"
#include "v8tree/Service.h"
#include <G3D/vectorMath.h>
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

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, long value)
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

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, long& value)
		{
			stream.Write(value);
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

		static bool isBrickLocation(const G3D::Vector3& v, short& x, unsigned short& y, short& z)
		{
			if (v.x >= 512.0f)
				return false;

			if (v.x <= -512.0f)
				return false;

			if (v.z >= 512.0f)
				return false;

			if (v.z <= -512.0f)
				return false;
			
			if (v.y >= 204.8)
				return false;

			if (v.y < 0.0f)
				return false;

			float dx = v.x * 2.0f;
			x = (short)dx;

			if ((float)x != dx)
				return false;

			float dz = v.z * 2.0f;
			z = (short)dz;

			if ((float)z != dz)
				return false;

			float dy = v.y * 10.0f;
			y = (unsigned short)dy;

			if (!brickEq((float)y, dy))
				return false;

			return true;
		}

		void writeBrickVector(RakNet::BitStream& stream, const G3D::Vector3& value)
		{
			short x;
			unsigned short y;
			short z;

			if (isBrickLocation(value, x, y, z))
			{
				stream.Write(true);
				stream.WriteBits((unsigned char*)&x, 11);
				stream.WriteBits((unsigned char*)&y, 11);
				stream.WriteBits((unsigned char*)&z, 11);
			}
			else
			{
				stream.Write(false);
				stream.Write(value.x);
				stream.Write(value.y);
				stream.Write(value.z);
			}
		}

		void readBrickVector(RakNet::BitStream& stream, G3D::Vector3& value)
		{
			bool isBrickLocation;
			stream.Read(isBrickLocation);

			if (isBrickLocation)
			{
				short x = 0;
				unsigned short y = 0;
				short z = 0;

				stream.ReadBits((unsigned char*)&x, 11);
				stream.ReadBits((unsigned char*)&y, 11);
				stream.ReadBits((unsigned char*)&z, 11);

				if (x & 0x400) // sign extend x and z
					x |= 0xfc00;

				if (z & 0x400)
					z |= 0xfc00;

				value.x = (float)(x * 0.5);
				value.y = (float)(y / 10.0);
				value.z = (float)(z * 0.5);
			}
			else
			{
				stream.Read(value.x);
				stream.Read(value.y);
				stream.Read(value.z);
			}
		}

		void rationalize(G3D::CoordinateFrame& value)
		{
			if (!value.translation.isFinite())
			{
				value.translation = G3D::Vector3(0.0f, -1e+6f, 0.0f);
			}
			else
			{
				value.translation = G3D::clamp(G3D::Vector3(-1e+6f, -1e+6f, -1e+6f), value.translation, G3D::Vector3(1e+6f, 1e+6f, 1e+6f));
			}
		}

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

		void StringSender::send(RakNet::BitStream& stream, const std::string& value)
		{
			if (value == "")
			{
				stream << (unsigned char)0;
			}
			else
			{
				std::pair<std::map<std::string, unsigned char>::iterator, bool> pair = dictionary.insert(std::pair<const std::string, unsigned char>(value, 0));

				if (!pair.second)
				{
					stream << pair.first->second;
				}
				else
				{
					lastIndex = lastIndex % 127 + 1;
					dictionary.erase(strings[lastIndex]);
					pair.first->second = lastIndex;
					strings[lastIndex] = value;

					stream << (unsigned char)(lastIndex | 0x80);
					stream << value;
				}
			}
		}

		bool StringSender::trySend(RakNet::BitStream& stream, const std::string& value)
		{
			if (value == "")
			{
				stream << (unsigned char)0;
				return true;
			}

			std::map<std::string, unsigned char>::const_iterator iter = dictionary.find(value);

			if (iter == dictionary.end())
			{
				return false;
			}
			else
			{
				stream << iter->second;
				return true;
			}
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
