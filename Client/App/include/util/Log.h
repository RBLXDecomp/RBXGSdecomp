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
		void writeEntry(Severity severity, const char* message);
		void timeStamp(bool);
	public:
		Log(const char* logFile, const char* name);
		virtual ~Log();

	public:
		static std::string formatMem(unsigned bytes);
		static std::string formatTime(double time);
		static void setLogProvider(ILogProvider* provider);
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
