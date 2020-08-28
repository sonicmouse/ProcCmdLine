#include "ProcessHelper.h"

PPEB CProcessHelper::GetPebAddress(HANDLE hProcess){

	static auto hModNTDLL = GetModuleHandleW(L"ntdll.dll");
	if (hModNTDLL) {
		static auto pNtQueryInformationProcess =
			reinterpret_cast<_NtQueryInformationProcess>(
				GetProcAddress(hModNTDLL, "NtQueryInformationProcess"));
		if (pNtQueryInformationProcess) {
			PROCESS_BASIC_INFORMATION pbi = { 0 };
			if (pNtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), nullptr) == 0) {
				return pbi.PebBaseAddress;
			}
		}
	}
	return nullptr;
}

int CProcessHelper::GetProcessCommandLine(DWORD dwProcId, LPWSTR& szCmdLine){
	szCmdLine = nullptr;
	auto rc = 0;

	auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProcess) {

		auto pPEBAddress = GetPebAddress(hProcess);
		if (pPEBAddress) {

			PRTL_USER_PROCESS_PARAMETERS pRtlUserProcParamsAddress;
			SIZE_T nSizeRead = 0;
			if (ReadProcessMemory(hProcess, &(pPEBAddress->ProcessParameters),
				&pRtlUserProcParamsAddress, sizeof(pRtlUserProcParamsAddress), &nSizeRead) &&
				(nSizeRead == sizeof(pRtlUserProcParamsAddress))) {

				UNICODE_STRING cmdLine;
				if (ReadProcessMemory(hProcess, &(pRtlUserProcParamsAddress->CommandLine),
					&cmdLine, sizeof(cmdLine), &nSizeRead) && (nSizeRead == sizeof(cmdLine))) {
					
					szCmdLine = new WCHAR[cmdLine.MaximumLength];
					if (szCmdLine) {
						memset(szCmdLine, 0, cmdLine.MaximumLength);

						if (ReadProcessMemory(hProcess, cmdLine.Buffer,
							szCmdLine, cmdLine.Length, &nSizeRead) && (nSizeRead == cmdLine.Length)) {
							rc = 0;
						} else {
							// couldn't read cmdLine
							delete[] szCmdLine;
							szCmdLine = nullptr;
							rc = -6;
						}
					} else {
						// couldn't allocate memory
						rc = -5;
					}
				} else {
					// couldn't read commandLine
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
