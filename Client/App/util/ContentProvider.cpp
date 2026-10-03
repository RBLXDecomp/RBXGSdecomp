#include "util/ContentProvider.h"
#include "util/standardout.h"
#include "util/Http.h"
#include "util/FileSystem.h"
#include "v8xml/SerializerV2.h"
#include <atlutil.h>
#include <G3D/format.h>
#include <boost/shared_ptr.hpp>
#include <boost/iostreams/copy.hpp>

namespace RBX
{
	// TODO: check match
	bool operator<(const ContentId& a, const ContentId& b)
	{
		return a.toString() < b.toString();
	}

	bool operator!=(const ContentId& a, const ContentId& b)
	{
		return a.toString() != b.toString();
	}

	ContentId ContentId::fromAssets(const std::string& filePath)
	{
		std::string header = "rbxasset://";
		return ContentId(header + filePath);
	}

	ContentProvider::ContentProvider()
	{
		char path[MAX_PATH];
		if (SHGetFolderPathAndSubDirA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, "Roblox\\content\\", (LPSTR)&path) == S_OK)
		{
			this->singleton().setAssetFolder(path);
		}
		else
		{
			StandardOut::singleton()->print(MESSAGE_WARNING, "Failed to get asset folder");
		}

		requestProcessor.reset(new worker_thread(boost::bind(&ContentProvider::processRequests, this), "rbx_content"));
	}

	ContentProvider::~ContentProvider()
	{
		requestProcessor->join();
	}

	bool ContentProvider::isHttpUrl(const std::string& s)
	{
		if (s.find("http://") == 0)
			return true;

		if (s.find("https://") == 0)
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

	bool ContentProvider::isUrlBad(const char* url)
	{
		boost::mutex::scoped_lock lock(requestSync);

		std::list<FailedUrl>::iterator iter = failedUrls.begin();
		std::list<FailedUrl>::iterator end = failedUrls.end();

		for (; iter != end; iter++)
		{
			if (boost::posix_time::second_clock::local_time() >= iter->expiration)
			{
				failedUrls.erase(iter, end);
				return false;
			}
			else if (iter->url == url)
			{
				return true;
			}
		}

		return false;
	}

	void ContentProvider::clearContentCache()
	{
		boost::mutex::scoped_lock lock(contentCacheMutex);
		contentCache.clear();
	}

	void ContentProvider::load(ContentId id, std::vector<boost::shared_ptr<Instance>>& instances)
	{
		boost::scoped_ptr<std::istream> stream(getContent(id));
		
		TextXmlParser machine(stream->rdbuf());
		boost::scoped_ptr<XmlElement> root(machine.parse());

		SerializerV2().loadInstances(root.get(), instances);
	}

	bool ContentProvider::hasContent(ContentId id)
	{
		return loadContent(id, NoHttpRequest) != NULL;
	}

	std::string ContentProvider::getAssetFile(const std::string& filePath)
	{
		return getFile(ContentId::fromAssets(filePath));
	}

	std::string ContentProvider::getFile(ContentId ticket)
	{
		CachedContent* content = loadContent(ticket, SyncHttpRequest);
		if (content && registerFile(content))
		{
			return *content->filename;
		}

		throw std::runtime_error(G3D::format("Unable to load %s", ticket.c_str()));
	}

	bool ContentProvider::requestContentFile(ContentId id, std::string& filename)
	{
		CachedContent* content = loadContent(id, AsyncHttpRequest);
		if (!content)
			return false;

		if (!registerFile(content))
			return false;

		filename = *content->filename;
		return true;
	}

	boost::shared_ptr<const std::string> ContentProvider::requestContentString(ContentId id)
	{
		CachedContent* content = loadContent(id, AsyncHttpRequest);
		if (!content)
			return boost::shared_ptr<const std::string>();

		boost::mutex::scoped_lock lock(contentCacheMutex);

		if (!content->data)
		{
			std::ifstream stream(content->filename->c_str(), std::ios::binary | std::ios::in);
			std::ostringstream data;

			boost::iostreams::copy(stream, data);

			content->data.reset(new std::string(data.str()));
		}

		return content->data;
	}

	std::string ContentProvider::findAsset(ContentId contentId)
	{
		RBXASSERT(contentId.isAsset());

		ATL::CPath filePath((assetFolderPath + (contentId.c_str() + 11)).c_str());
		if (!filePath.FileExists())
			return "";

		return std::string(filePath);
	}

	std::string ContentProvider::findFile(ContentId contentId)
	{
		RBXASSERT(contentId.isFile());

		const char* file = contentId.c_str();
		file += 7;

		ATL::CPath filePath(file);
		if (!filePath.FileExists())
			return "";

		return std::string(filePath);
	}

	static ATL::CPath getLocalCachePath(bool createPath)
	{
		ATL::CPath path;

		if (createPath)
		{
			static std::string s = FileSystem::getCacheDirectory(true);
			path.m_strPath.SetString(s.c_str());
		}
		else
		{
			static std::string s = FileSystem::getCacheDirectory(false);
			path.m_strPath.SetString(s.c_str());
		}

		return path;
	}

	std::string ContentProvider::findHashFile(ContentId contentId)
	{
		ATL::CPath cachePath = getLocalCachePath(true);

		ATL::CPath filePath = cachePath;
		filePath.Append(contentId.c_str());

		if (!filePath.FileExists())
			return "";

		return std::string(filePath);
	}

	void ContentProvider::setAssetFolder(const char* sPath)
	{
		StandardOut::singleton()->print(MESSAGE_INFO, "setAssetFolder %s", sPath);

		ATL::CPath path(sPath);

		if (path.IsRelative())
			throw std::runtime_error(G3D::format("The path \'%s\' is a relative path", path));

		if (!path.IsDirectory())
			throw std::runtime_error(G3D::format("The path \'%s\' does not exist", path));

		path.AddBackslash();
		assetFolderPath = path;
	}

	ContentProvider::FailedUrl::FailedUrl(const char* url)
		: url(url),
		  expiration(boost::posix_time::second_clock::local_time() + boost::posix_time::minutes(5))
	{
	}

	class MD5HasherImpl : public MD5Hasher
	{
	private:
		HCRYPTPROV hProv;
		HCRYPTHASH hHash;
		std::string result;

	public:
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
	};

	MD5Hasher* MD5Hasher::create()
	{
		return new MD5HasherImpl();
	}
}
