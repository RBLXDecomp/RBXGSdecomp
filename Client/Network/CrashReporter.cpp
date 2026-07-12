#include "CrashReporter.h"
#include <tchar.h>
#include <stdio.h>
#include <FileOperations.h>

LONG WINAPI CrashExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	return CrashReporter::singleton->ProcessException(ExceptionInfo);
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
	LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}

// for the following function and the one above, see: http://blog.kalmbachnet.de/?postid=75

#pragma warning (push)
#pragma warning (disable : 4311) // warning C4311: 'type cast' : pointer truncation from 'void *' to 'DWORD'

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if(pOrgEntry == NULL) return FALSE;
	unsigned char newJump[ 100 ];
	DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD) pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[ 0 ] = 0xE9;  // JMP absolute
	memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
	return bRet;
}

#pragma warning (pop)

void CrashReporter::Start()
{
	SetUnhandledExceptionFilter(&CrashExceptionFilter);
	PreventSetUnhandledExceptionFilter();
}

LONG CrashReporter::ProcessException(EXCEPTION_POINTERS* ExceptionInfo)
{
	char appDescriptor[260];
	char dumpFilename[260];
	char dumpFilepath[260];

	if ((controls.actionToTake & 8) == 0)
	{
		sprintf(appDescriptor, "%s has crashed.\nGenerate a report?", controls.appName);
		if (MessageBoxA(NULL, appDescriptor, "Crash Reporter", MB_YESNO) == IDNO)
			return EXCEPTION_CONTINUE_SEARCH;
	}

	sprintf(appDescriptor, "%s %s", controls.appName, controls.appVersion);

	if ((controls.actionToTake & 4) != 0)
	{
		strcpy(dumpFilepath, controls.pathToMinidump);
		WriteFileWithDirectories(dumpFilepath, NULL, 0);
		AddSlash(dumpFilepath);

		strcpy(dumpFilename, appDescriptor);

		size_t len = strlen(appDescriptor);
		for (size_t i = 0; i < len; i++)
		{
			if (dumpFilename[i] == ':')
				dumpFilename[i] = '.';
		}

		strcat(dumpFilepath, dumpFilename);
		strcat(dumpFilepath, ".dmp");

		HANDLE file = CreateFileA(dumpFilepath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return EXCEPTION_CONTINUE_SEARCH;

		MINIDUMP_EXCEPTION_INFORMATION eInfo;

		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = ExceptionInfo;
		eInfo.ClientPointers = FALSE;

		LONG result = EXCEPTION_EXECUTE_HANDLER;

		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, controls.minidumpType, (ExceptionInfo != NULL ? &eInfo : NULL), NULL, NULL))
			result = EXCEPTION_CONTINUE_SEARCH;

		CloseHandle(file);
		return result;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
