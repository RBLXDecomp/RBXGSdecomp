#include "util/FileSystem.h"
#include <atlutil.h>

namespace RBX
{
	VistaAPIs::VistaAPIs()
		: getKnownFolderPath(NULL)
	{
		gShell32DLLInst = LoadLibraryA("Shell32.dll");
		if (gShell32DLLInst)
		{
			getKnownFolderPath = (GetKnownFolderPathPtr)GetProcAddress(gShell32DLLInst, "SHGetKnownFolderPath");
		}
	}

	VistaAPIs::~VistaAPIs()
	{
		if (gShell32DLLInst)
			FreeLibrary(gShell32DLLInst);
	}

	HRESULT VistaAPIs::SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath)
	{
		if (!getKnownFolderPath)
			return E_NOTIMPL;

		return getKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
	}

	HRESULT VistaAPIs::GetIntegrity(PUCHAR pVal)
	{
		*pVal = 0;

		OSVERSIONINFOA osvi = {0};
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		GetVersionExA(&osvi);
		
		if (osvi.dwMajorVersion < 6) // anything earlier than Windows Vista
			return S_OK;

		HANDLE hToken;
		HANDLE hProcess = GetCurrentProcess();
		if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken))
			return S_OK;

		DWORD dwLengthNeeded;
		if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0L, &dwLengthNeeded))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				PTOKEN_MANDATORY_LABEL pTIL;
				pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(LMEM_FIXED, dwLengthNeeded);

				if (pTIL)
				{
					if (GetTokenInformation(hToken, TokenIntegrityLevel, pTIL, dwLengthNeeded, &dwLengthNeeded))
					{
						DWORD dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

						if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
							*pVal = 1;
						else if (dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
							*pVal = 2;
						else
							*pVal = 3;
					}

					LocalFree(pTIL);
				}
			}
		}

		CloseHandle(hToken);
		return S_OK;
	}

	std::string FileSystem::getCacheDirectory(bool create)
	{
		VistaAPIs apis;

		UCHAR integrity;
		apis.GetIntegrity(&integrity);

		if (integrity == 1)
		{
			KNOWNFOLDERID folderID;
			CLSIDFromString(OLESTR("{352481E8-33BE-4251-BA85-6007CAEDCF9D"), &folderID); // FOLDERID_InternetCache
			
			PWSTR lppwstrPath = NULL;
			if (apis.SHGetKnownFolderPath(folderID, KF_FLAG_CREATE, NULL, &lppwstrPath) != E_NOTIMPL)
			{
				ATL::CString s = lppwstrPath;
				CoTaskMemFree(lppwstrPath);
				s.Append("\\Roblox\\");

				if (create)
				{
					if (!CreateDirectoryA(s, NULL))
					{
						if (GetLastError() != ERROR_ALREADY_EXISTS)
							return "";
					}
				}

				ATL::CPath path = s;
				path.AddBackslash();
				return std::string(path);
			}
		}

		char path[MAX_PATH];
		if (SHGetFolderPathAndSubDirA(NULL, (create ? CSIDL_FLAG_CREATE : 0L) | CSIDL_COMMON_APPDATA, NULL, 0L, "Roblox\\Cache", path) == S_OK)
			return path;

		return "";
	}
}
