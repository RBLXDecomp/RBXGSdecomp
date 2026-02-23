#include "util/ContentProvider.h"
#include "util/standardout.h"
#include "util/Http.h"
#include <atlutil.h>
#include <boost/shared_ptr.hpp>

namespace RBX
{
	// TODO: check match
	bool operator<(const ContentId& a, const ContentId& b)
	{
		return a.toString() < b.toString();
	}

	ContentProvider& ContentProvider::singleton()
	{
		static ContentProvider sing;
		return sing;
	}

	bool ContentProvider::isHttpUrl(const std::string& s)
	{
		if (s.find("http://", 0, 7) == 0)
			return true;

		if (s.find("https://", 0, 8) == 0)
			return true;

		return false;
	}

	std::string ContentProvider::assetFolder() const
	{
		return assetFolderPath;
	}

	bool ContentProvider::isRequestQueueEmpty()
	{
		boost::mutex::scoped_lock lock(requestSync);
		return requestQueue.empty();
	}

	class MD5HasherImpl : public MD5Hasher
	{
	private:
		HCRYPTPROV hProv;
		HCRYPTHASH hHash;
		std::string result;

	public:
		//MD5HasherImpl(const MD5HasherImpl&);
		MD5HasherImpl()
			: hProv(NULL),
			  hHash(NULL),
			  result()
		{
			if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
			{
				StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptAcquireContext. GetLastError = %d", GetLastError());
			}

			if (!CryptCreateHash(hProv, CALG_MD5, NULL, NULL, &hHash))
			{
				StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptCreateHash. GetLastError = %d", GetLastError());
			}
		}
		~MD5HasherImpl()
		{
		}

	public:
		virtual void addData(const char* data, size_t nBytes)
		{
			if (!CryptHashData(hHash, (const BYTE*)data, (DWORD)nBytes, 0))
			{
				StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptHashData. GetLastError = %d", GetLastError());
			}
		}
		virtual void addData(const std::string& data)
		{
			if (!CryptHashData(hHash, (const BYTE*)data.c_str(), (DWORD)data.length(), 0))
			{
				StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptHashData. GetLastError = %d", GetLastError());
			}
		}
		virtual void addData(std::istream& data)
		{
			data.clear();
			data.seekg(0, std::ios_base::beg);

			char buffer[1024];
			do
			{
				data.read(buffer, sizeof(buffer));
				addData(buffer, data.gcount());
			}
			while (data.gcount() > 0);
		}
		virtual std::string toString()
		{
			c_str();
			return result;
		}
		virtual const char* c_str()
		{
			if (result.empty())
			{
				DWORD hashSize;
				DWORD hashSizeLen = sizeof(DWORD);
				if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashSize, &hashSizeLen, 0))
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptGetHashParam. GetLastError = %d", GetLastError());
				}

				char* hashValue = new char[hashSize];
				if (!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)&hashValue, &hashSize, 0))
				{
					StandardOut::singleton()->print(MESSAGE_ERROR, "Error during CryptGetHashParam. GetLastError = %d", GetLastError());
				}

				for (int i = 0; i < (int)hashSize; ++i)
				{
					ATL::CString temp;
					temp.Format("%x", hashValue[i]);
					result += temp.GetString();
				}

				delete[] hashValue;
			}

			return result.c_str();
		}
	public:
		//MD5HasherImpl& operator=(const MD5HasherImpl&);
	};

	MD5Hasher* MD5Hasher::create()
	{
		return new MD5HasherImpl();
	}

	bool operator!=(const ContentId& a, const ContentId& b) 
	{
		return a.toString() != b.toString();
	}
}
