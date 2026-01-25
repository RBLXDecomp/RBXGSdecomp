#pragma once
#include "util/Name.h"
#include "util/boost.hpp"
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

namespace RBX
{
	class ContentId;
	bool operator!=(const ContentId&, const ContentId&);

	class ContentId
	{
	private:
		std::string id;
		const Name* mimeTypePtr;

	public:
		ContentId(const ContentId& other)
			: id(other.id),
			  mimeTypePtr(other.mimeTypePtr)
		{
		}
		ContentId()
			: id(), 
			  mimeTypePtr(&Name::getNullName())
		{
		}
		ContentId(std::string id)
			: id(id),
			  mimeTypePtr(&Name::getNullName())
		{
		}
		ContentId(const char* id)
			: id(id),
			  mimeTypePtr(&Name::getNullName())
		{
		}
		ContentId(std::string id, const Name& mimeType)
			: id(id),
			  mimeTypePtr(&mimeType)
		{
		}

	public:
		const Name& mimeType() const
		{
			return *mimeTypePtr;
		}
		const char* c_str() const
		{
			return id.c_str();
		}
		const std::string& toString() const
		{
			return id;
		}
		bool isNull() const
		{
			return id.empty();
		}
		bool isFile() const
		{
			return id.substr(0, 7) == "file://";
		}
		bool isAsset() const
		{
			return id.substr(0, 11) == "rbxasset://";
		}
		bool isHttp() const
		{
			return id.substr(0, 4) == "http";
		}

	public:
		~ContentId() 
		{
		}

	public:
		static ContentId fromUrl(const std::string&);
		static ContentId fromFile(const std::string&);
		static ContentId fromAssets(const std::string&);
		static ContentId fromMD5Hash(const std::string&);
	};

	class Instance;
	class ContentProvider : private boost::noncopyable
	{
	private:
		enum HttpRequestType
		{
			NoHttpRequest,
			AsyncHttpRequest,
			SyncHttpRequest
		};

		struct CachedContent
		{
		public:
			boost::shared_ptr<const std::string> data;
			boost::shared_ptr<const std::string> filename;
		  
		public:
			//CachedContent(const CachedContent&);
			CachedContent();
			~CachedContent();
		public:
			//CachedContent& operator=(const CachedContent&);
		};

		struct FailedUrl
		{
		public:
			std::string url;
			boost::posix_time::ptime expiration;
		  
		public:
			//FailedUrl(const FailedUrl&);
			FailedUrl(const char*);
		public:
			bool expired() const;
		public:
			~FailedUrl();
		public:
			//FailedUrl& operator=(const FailedUrl&);
		};

	private:
		boost::mutex requestSync;
		std::list<ContentId> requestQueue;
		std::list<FailedUrl> failedUrls;
		boost::mutex contentCacheMutex;
		std::map<ContentId, CachedContent> contentCache;
		std::string assetFolderPath;
		boost::scoped_ptr<worker_thread> requestProcessor;
	  
	public:
		//ContentProvider(const ContentProvider&);
	private:
		ContentProvider();
		~ContentProvider();

	public:
		ContentId registerContent(std::istream& content, const Name& mimeType);
		ContentId registerContent(const char*, const Name&);
		void clearFileCache();
		bool isUrlBad(const char*);
		void load(ContentId, std::vector<boost::shared_ptr<Instance>>&);
		void clearContentCache();
		bool isRequestQueueEmpty();
		bool hasContent(ContentId);
		boost::shared_ptr<const std::string> requestContentString(ContentId);
		bool requestContentFile(ContentId, std::string&);
		boost::shared_ptr<const std::string> getContentString(ContentId);
		std::auto_ptr<std::istream> getContent(ContentId);
		std::string getFile(ContentId);
		std::string getAssetFile(const std::string&);
		void setAssetFolder(const char*);
		std::string assetFolder() const;
		ContentId readContent(const char*, std::istream&, unsigned);
	private:
		RBX::ContentProvider::CachedContent* loadContent(ContentId, HttpRequestType);
		worker_thread::work_result processRequests();
		std::string findFile(ContentId);
		std::string findAsset(ContentId);
		std::string findHashFile(ContentId);
		bool registerFile(CachedContent*);
	public:
		//ContentProvider& operator=(const ContentProvider&);
	  
	public:
		static ContentProvider& singleton();
		static bool isUrl(const std::string&);
		static bool isHttpUrl(const std::string&);
	};

	class MD5Hasher
	{
	public:
		virtual void addData(const char* data, size_t nBytes) = 0;
		virtual void addData(const std::string& data) = 0;
		virtual void addData(std::istream& data) = 0;
		
		virtual std::string toString() = 0;
		virtual const char* c_str() = 0;

	public:
		//MD5Hasher(const MD5Hasher&);
		MD5Hasher()
		{
		}
	public:
		//MD5Hasher& operator=(const MD5Hasher&);

	public:
		static MD5Hasher* create();
	};
}
