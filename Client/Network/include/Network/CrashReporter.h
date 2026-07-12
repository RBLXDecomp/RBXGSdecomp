#pragma once
#include <windows.h>
#include <dbghelp.h>

struct CrashReportControls
{
	int actionToTake;
	char appName[128];
	char appVersion[128];
	char pathToMinidump[260];
	char SMTPServer[128];
	char SMTPAccountName[64];
	char emailSender[64];
	char emailSubjectPrefix[128];
	char emailBody[1024];
	char emailRecipient[64];
	MINIDUMP_TYPE minidumpType;
};

class CrashReporter
{
public:
	CrashReportControls controls;
	static CrashReporter* singleton;

	CrashReporter();
	void Start();
	virtual LONG ProcessException(EXCEPTION_POINTERS* ExceptionInfo);
};
