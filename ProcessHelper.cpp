#include "ProcessHelper.h"

PPEB ProcessHelper::GetPebAddress(HANDLE hProcess) {

	PROCESS_BASIC_INFORMATION pbi = { 0 };

	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr))) {
		return pbi.PebBaseAddress;
	} else {
		return nullptr;
	}
}

// Source: https://www.vergiliusproject.com/kernels/x64/Windows%20XP%20%7C%202003/SP2/_RTL_DRIVE_LETTER_CURDIR
//0x18 bytes (sizeof)
struct MY_RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;                                                           //0x0
	USHORT Length;                                                          //0x2
	ULONG TimeStamp;                                                        //0x4
	struct _STRING DosPath;                                                 //0x8
};

// Source: https://www.vergiliusproject.com/kernels/x64/Windows%20XP%20%7C%202003/SP2/_CURDIR
//0x18 bytes (sizeof)
struct MY_CURDIR
{
	struct _UNICODE_STRING DosPath;                                         //0x0
	VOID* Handle;                                                           //0x10
};

// Source: https://www.vergiliusproject.com/kernels/x64/Windows%20XP%20%7C%202003/SP2/_RTL_USER_PROCESS_PARAMETERS
//0x3f0 bytes (sizeof)
struct MY_RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;                                                    //0x0
	ULONG Length;                                                           //0x4
	ULONG Flags;                                                            //0x8
	ULONG DebugFlags;                                                       //0xc
	VOID* ConsoleHandle;                                                    //0x10
	ULONG ConsoleFlags;                                                     //0x18
	VOID* StandardInput;                                                    //0x20
	VOID* StandardOutput;                                                   //0x28
	VOID* StandardError;                                                    //0x30
	struct MY_CURDIR CurrentDirectory;                                      //0x38
	struct _UNICODE_STRING DllPath;                                         //0x50
	struct _UNICODE_STRING ImagePathName;                                   //0x60
	struct _UNICODE_STRING CommandLine;                                     //0x70
	VOID* Environment;                                                      //0x80
	ULONG StartingX;                                                        //0x88
	ULONG StartingY;                                                        //0x8c
	ULONG CountX;                                                           //0x90
	ULONG CountY;                                                           //0x94
	ULONG CountCharsX;                                                      //0x98
	ULONG CountCharsY;                                                      //0x9c
	ULONG FillAttribute;                                                    //0xa0
	ULONG WindowFlags;                                                      //0xa4
	ULONG ShowWindowFlags;                                                  //0xa8
	struct _UNICODE_STRING WindowTitle;                                     //0xb0
	struct _UNICODE_STRING DesktopInfo;                                     //0xc0
	struct _UNICODE_STRING ShellInfo;                                       //0xd0
	struct _UNICODE_STRING RuntimeData;                                     //0xe0
	struct MY_RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];                //0xf0
};

int ProcessHelper::GetProcessParameter(DWORD dwProcId, LPWSTR& szParameter, ProcessParameter parameter) {
	szParameter = nullptr;
	auto rc = 0;

	auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProcess) {

		auto pPEBAddress = GetPebAddress(hProcess);
		if (pPEBAddress) {

			MY_RTL_USER_PROCESS_PARAMETERS* pRtlUserProcParamsAddress;
			SIZE_T nSizeRead = 0;
			if (ReadProcessMemory(hProcess, &(pPEBAddress->ProcessParameters),
				&pRtlUserProcParamsAddress, sizeof(pRtlUserProcParamsAddress), &nSizeRead) &&
				(nSizeRead == sizeof(pRtlUserProcParamsAddress))) {

				auto paramPtr = [parameter, pRtlUserProcParamsAddress]() -> PUNICODE_STRING
				{
					switch (parameter)
					{
					case ProcessParameter::CommandLine:
						return &pRtlUserProcParamsAddress->CommandLine;
					case ProcessParameter::WorkingDirectory:
						return &pRtlUserProcParamsAddress->CurrentDirectory.DosPath;
					default:
						return nullptr;
					}
				}();
				UNICODE_STRING paramStr;
				if (ReadProcessMemory(hProcess, paramPtr,
					&paramStr, sizeof(paramStr), &nSizeRead) && (nSizeRead == sizeof(paramStr))) {
					
					szParameter = new WCHAR[paramStr.MaximumLength];
					if (szParameter) {
						memset(szParameter, 0, paramStr.MaximumLength);

						if (ReadProcessMemory(hProcess, paramStr.Buffer,
							szParameter, paramStr.Length, &nSizeRead) && (nSizeRead == paramStr.Length)) {
							rc = 0;
						} else {
							// couldn't read parameter string
							delete[] szParameter;
							szParameter = nullptr;
							rc = -6;
						}
					} else {
						// couldn't allocate memory
						rc = -5;
					}
				} else {
					// couldn't read parameter
					rc = -4;
				}
			} else {
				// couldn't read process memory
				rc = -3;
			}
		} else {
			// couldn't get PEB address
			rc = -2;
		}
		CloseHandle(hProcess);
	} else {
		// couldn't open process
		rc = -1;
	}

	return rc;
}
