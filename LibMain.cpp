#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h> // for wcstombs_s()
#include "ProcessHelper.h"

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved){
	if (fdwReason == DLL_PROCESS_ATTACH){
		DisableThreadLibraryCalls(hInstDLL);
	}
	return TRUE;
}

int __stdcall GetProcCmdLineA(DWORD dwProcId, PCHAR buf, DWORD dwSizeBuf) {

	LPWSTR psz = nullptr;
	const auto rc = CProcessHelper::GetProcessCommandLine(dwProcId, psz);

	if ((0 == rc) && psz) {
		size_t conv;
		wcstombs_s(&conv, buf, dwSizeBuf, psz, dwSizeBuf);
	}

	if (psz) { delete[] psz; }

	return rc;
}

int __stdcall GetProcCmdLineW(DWORD dwProcId, PWCHAR buf, DWORD dwSizeBuf){

	LPWSTR psz = nullptr;
	const auto rc = CProcessHelper::GetProcessCommandLine(dwProcId, psz);

	if ((0 == rc) && psz) {
		wcscpy_s(buf, dwSizeBuf, psz);
	}

	if(psz){ delete[] psz; }

	return rc;
}
