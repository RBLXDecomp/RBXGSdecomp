#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include "util/Guid.h"
#include "util/Debug.h"
#include <windows.h>
#include <limits>
#include <boost/thread/once.hpp>

const RBX::Name* localScope = NULL;
LONG nextIndex = 0;

void initLocalScope()
{
	std::string guid;
	RBX::Guid::generateGUID(guid);
	localScope = &RBX::Name::declare(guid.c_str(), -1);
}

namespace RBX
{
	Guid::Guid()
	{
		static boost::once_flag flag = BOOST_ONCE_INIT;
		boost::call_once(initLocalScope, flag);
		this->data.scope = localScope;
		this->data.index = InterlockedIncrement(&nextIndex);
	}

	std::string Guid::readableString(int scopeLength) const
	{
		return this->data.readableString(4);
	}

	void Guid::assign(Guid::Data data)
	{
		RBXASSERT(this->data.scope == localScope);
		this->data = data;
	}

	void Guid::generateGUID(std::string& result)
	{
		result = "RBX";

		GUID guid;
		CoCreateGuid(&guid);

		wchar_t wszGUID[64] = {0};
		StringFromGUID2(guid, wszGUID, 64);

		char ansiClsid[64];
		WideCharToMultiByte(NULL, NULL, wszGUID, 64, ansiClsid, 64, NULL, NULL);
		
		result += ansiClsid;
		result.erase(40, 1);
		result.erase(27, 1);
		result.erase(22, 1);
		result.erase(17, 1);
		result.erase(12, 1);
		result.erase(3, 1);
	}

	std::string Guid::Data::readableString(int scopeLength) const
	{
		char result[64];

		if (scopeLength > 0)
		{
			std::string scopeName = this->scope->name;
			scopeName = scopeName.substr(std::min((size_t)3, scopeName.size()), std::min(scopeLength, 32));
			sprintf(result, "%s_%d", scopeName.c_str(), this->index);
		}
		else
		{
			sprintf(result, "%d", this->index);
		}

		return result;
	}

	bool Guid::Data::operator <(const Data& other) const
	{
		switch (this->scope->compare(*other.scope))
		{
		case -1:
			return true;
		case 0:
			return this->index < other.index;
		case 1:
			return false;
		default:
			RBXASSERT(0);
			return false;
		}
	}
}
