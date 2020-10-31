#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h> // for wcstombs_s()
#include "ProcessHelper.h"

using namespace ProcessHelper;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hInstDLL);
	}
	return TRUE;
}

int GetProcessParameterA(DWORD dwProcId, PCHAR buf, DWORD dwSizeBuf, ProcessParameter parameter) {

	LPWSTR psz = nullptr;
	const auto rc = GetProcessParameter(dwProcId, psz, parameter);

	if ((0 == rc) && psz) {
		size_t conv;
		wcstombs_s(&conv, buf, dwSizeBuf, psz, dwSizeBuf);
	}

	if (psz) { delete[] psz; }

	return rc;
}

static int GetProcessParameterW(DWORD dwProcId, PWCHAR buf, DWORD dwSizeBuf, ProcessParameter parameter) {

	LPWSTR psz = nullptr;
	const auto rc = GetProcessParameter(dwProcId, psz, parameter);

	if ((0 == rc) && psz) {
		wcscpy_s(buf, dwSizeBuf, psz);
	}

	if (psz) { delete[] psz; }

	return rc;
}

int __stdcall GetProcCmdLineA(DWORD dwProcId, PCHAR buf, DWORD dwSizeBuf) {
	return GetProcessParameterA(dwProcId, buf, dwSizeBuf, ProcessParameter::CommandLine);
}

int __stdcall GetProcCmdLineW(DWORD dwProcId, PWCHAR buf, DWORD dwSizeBuf) {
	return GetProcessParameterW(dwProcId, buf, dwSizeBuf, ProcessParameter::CommandLine);
}

int __stdcall GetProcWorkingDirA(DWORD dwProcId, PCHAR buf, DWORD dwSizeBuf) {
	return GetProcessParameterA(dwProcId, buf, dwSizeBuf, ProcessParameter::WorkingDirectory);
}

int __stdcall GetProcWorkingDirW(DWORD dwProcId, PWCHAR buf, DWORD dwSizeBuf) {
	return GetProcessParameterW(dwProcId, buf, dwSizeBuf, ProcessParameter::WorkingDirectory);
}