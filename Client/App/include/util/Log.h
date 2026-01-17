#pragma once
#include "util/Debug.h"
#include <string>
#include <fstream>

namespace RBX
{
	class Log;

	class ILogProvider
	{
	public:
		virtual Log* provideLog() = 0;
	public:
		//ILogProvider(const ILogProvider&);
		ILogProvider();
	public:
		//ILogProvider& operator=(const ILogProvider&);
	};

	class Log
	{
	public:
		enum Severity
		{
			Information,
			Warning,
			Error
		};

	public:
		Severity worstSeverity;
		const std::string logFile;
	private:
		std::ofstream stream;
	public:
		static Severity aggregateWorstSeverity;
	private:
		static ILogProvider* provider;

	public:
		void writeEntry(Severity, const char*);
		void timeStamp(bool);
	public:
		//Log(const Log&);
		Log(const char*, const char*);
		virtual ~Log();
	public:
		//Log& operator=(const Log&);

	public:
		static std::string formatMem(unsigned);
		static std::string formatTime(double);
		static void setLogProvider(ILogProvider*);
		static Log* current()
		{
			return provider ? provider->provideLog() : NULL;
		}
	private:
		static std::ofstream& currentStream()
		{
			RBXASSERT(provider->provideLog());
			return provider->provideLog()->stream;
		}
	};
}
