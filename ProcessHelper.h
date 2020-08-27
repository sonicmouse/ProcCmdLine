#ifndef __PROCESS_HELPER_H__
#define __PROCESS_HELPER_H__
#pragma once

#include <windows.h>
#include <winternl.h>

class CProcessHelper
{
public:
	static int GetProcessCommandLine(DWORD dwProcId, LPWSTR& szCmdLine);

private:
	static PPEB GetPebAddress(HANDLE hProcess);

	typedef NTSTATUS (NTAPI* _NtQueryInformationProcess)(
					  HANDLE ProcessHandle,
					  DWORD ProcessInformationClass,
					  PVOID ProcessInformation,
					  DWORD ProcessInformationLength,
					  PDWORD ReturnLength);
};

#endif /** !__PROCESS_HELPER_H__ **/