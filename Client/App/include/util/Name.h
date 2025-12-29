#pragma once
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include <map>

namespace RBX
{
	class Name : boost::noncopyable
	{
	private:
		typedef std::map<std::string, Name*> NamMap;

	private:
		int dictionaryIndex;
	public:
		const std::string name;

	public:
		int getDictionaryIndex() const { return dictionaryIndex; }

		bool empty() const { return &getNullName() == this; }
		const std::string& toString() const { return name; } // guess
		const char* c_str() const { return name.c_str(); } // guess
		int compare(const Name& other) const;

		bool operator <(const RBX::Name& other) const;
		bool operator >(const RBX::Name& other) const;
		bool operator ==(const char* name) const
		{
			return this->name.compare(name) == 0;
		}
		bool operator ==(const std::string& name) const
		{
			return this->name.compare(name) == 0;
		}
		bool operator ==(const Name& other) const
		{
			return this == &other;
		}
		bool operator !=(const char* name) const;
		bool operator !=(const std::string& name) const;
		bool operator !=(const Name& other) const
		{
			return this != &other;
		}

	private:
		Name(const char* sName, int dictionaryIndex)
			: name(sName),
			  dictionaryIndex(dictionaryIndex)
		{}
	public:
		~Name() {}

	private:
		static boost::mutex& mutex();
		static std::map<int, Name*>& dictionary();
		static NamMap& namMap();

	public:
		static const Name& getNullName();
		static const Name& declare(const char* sName, int dictionaryIndex);
		static const Name& lookup(int dictionaryIndex);
		static const Name& lookup(const char* sName);
		static const Name& lookup(const std::string& sName);
		static int compare(Name&, Name&);
	private:
		// NOTE: these have not been checked
		// TODO: these also need to support const char* inputs
		template<char*& sName>
		static const Name& doDeclare()
		{
			static const Name& n = declare(sName, -1);
			return n;
		}

		template<char*& sName>
		static const Name& callDoDeclare()
		{
			return doDeclare<sName>();
		}
	};

	class INamed
	{
	public:
		virtual const Name& getName() const = 0;
	public:
		//INamed(const INamed&);
		INamed();
	public:
		//INamed& operator=(const INamed&);
	};
}
