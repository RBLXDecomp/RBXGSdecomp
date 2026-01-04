#include "v8datamodel/BrickColor.h"
#include "v8tree/Instance.h"
#include "util/ContentProvider.h"
#include "util/Name.h"
#include "util/Guid.h"
#include "reflection/property.h"
#include <BitStream.h>
#include <g3d/CoordinateFrame.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <map>
#include <string>

namespace RBX
{
	namespace Network
	{
		template<typename T>
		void serialize(const Reflection::ConstProperty&, RakNet::BitStream&);

		template<typename T>
		void deserialize(Reflection::Property&, RakNet::BitStream&);

		void serializeEnum(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream);
		void deserializeEnum(Reflection::Property& property, RakNet::BitStream& bitStream);

		RakNet::BitStream& operator<<(RakNet::BitStream& stream, bool value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, int value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, unsigned int value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, unsigned char value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, float value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const std::string& value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const G3D::Vector3& value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const G3D::Color3& value);
		RakNet::BitStream& operator<<(RakNet::BitStream& stream, const G3D::CoordinateFrame& cf);

		RakNet::BitStream& operator>>(RakNet::BitStream& stream, bool& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, int& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, unsigned int& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, unsigned char& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, float& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, std::string& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, G3D::Vector3& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, G3D::Color3& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, BrickColor& value);
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, ContentId& value);

		template<typename T>
		RakNet::BitStream& operator>>(RakNet::BitStream& stream, T& value); // TODO: check match

		void writeBrickVector(RakNet::BitStream&, const G3D::Vector3&);
		void readBrickVector(RakNet::BitStream& stream, G3D::Vector3& value);
		void rationalize(G3D::CoordinateFrame& value);

		class StringReceiver
		{
		private:
			std::map<unsigned char, std::string> dictionary;
		public:
			void deserializeString(Reflection::Property& property, RakNet::BitStream& bitStream);
			void receive(RakNet::BitStream& stream, const Name*& value);
			void receive(RakNet::BitStream& stream, std::string& value);
			//StringReceiver(const StringReceiver&);
			StringReceiver();
			~StringReceiver();
			//StringReceiver& operator=(const StringReceiver&);
		};

		class StringSender
		{
		private:
			std::map<std::string, unsigned char> dictionary;
			std::string strings[128];
			int lastIndex;
		public:
			//StringSender(const StringSender&);
			StringSender()
				: lastIndex(0)
			{
			}

			void serializeString(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream);

			void send(RakNet::BitStream&, const char*);
			void send(RakNet::BitStream&, const Name&);
			void send(RakNet::BitStream&, const std::string&);

			bool trySend(RakNet::BitStream&, const char*);
			bool trySend(RakNet::BitStream&, const Name&);
			bool trySend(RakNet::BitStream&, const std::string&);

			~StringSender();
			//StringSender& operator=(const StringSender&);
		};

		class SharedStringDictionary : public StringSender, public StringReceiver, private boost::noncopyable
		{
		};

		class IdSerializer : public Instance
		{
			struct WaitItem
			{
			public:
				const Reflection::RefPropertyDescriptor* desc;
				boost::shared_ptr<Instance> instance;
			};

		private:
			SharedStringDictionary scopeNames;
		protected:
			std::map<Guid::Data, std::vector<WaitItem>> waitItems;
		public:
			//IdSerializer(const IdSerializer&);
			IdSerializer() {}

			void serializeId(RakNet::BitStream& stream, const Instance* instance);
			bool trySerializeId(RakNet::BitStream& stream, const Instance* instance);
			void deserializeId(RakNet::BitStream& stream, Guid::Data& id);
			void resolvePendingBindings(Instance* instance, Guid::Data id);
			bool deserializeInstanceRef(RakNet::BitStream&, Instance*&);
			bool deserializeInstanceRef(RakNet::BitStream&, Instance*&, Guid::Data&);
			size_t numWaitingRefs() const;
			void serializeRef(const Reflection::ConstProperty& property, RakNet::BitStream& bitStream);
			void deserializeRef(Reflection::Property&, RakNet::BitStream&);
			virtual ~IdSerializer();
			//IdSerializer& operator=(const IdSerializer&);
		protected:
			static void setRefValue(WaitItem& wi, Instance* instance);
		};
	}
}
