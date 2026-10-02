#include "util/FileSystem.h"

namespace RBX
{
	VistaAPIs::~VistaAPIs()
	{
		if (gShell32DLLInst)
			FreeLibrary(gShell32DLLInst);
	}

	HRESULT VistaAPIs::GetIntegrity(PUCHAR pVal)
	{
		*pVal = 0;

		OSVERSIONINFOA osvi;
		ZeroMemory((char*)&osvi + sizeof(DWORD), sizeof(OSVERSIONINFOA) - sizeof(DWORD));
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
}
