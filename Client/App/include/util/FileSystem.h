#include <shlobj.h>
#include <string>

namespace RBX
{
	class VistaAPIs
	{
		typedef struct _TOKEN_MANDATORY_LABEL 
		{
			SID_AND_ATTRIBUTES Label;
		} TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

		typedef HRESULT (WINAPI *GetKnownFolderPathPtr)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);

	private:
		GetKnownFolderPathPtr getKnownFolderPath;
		HINSTANCE gShell32DLLInst;

	public:
		VistaAPIs();
		~VistaAPIs();
		HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);
		HRESULT __stdcall GetIntegrity(PUCHAR pVal); // why is this stdcall?
	};

	class FileSystem
	{
	public:
		static std::string getUserAppDataDirectory(bool create, const char* subDirectory);
		static std::string getCacheDirectory(bool create);
	};
}
