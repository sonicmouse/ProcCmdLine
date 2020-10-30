#pragma once

#include <windows.h>
#include <winternl.h>

namespace ProcessHelper
{
enum class ProcessParameter
{
	CommandLine,
	WorkingDirectory,
};

int GetProcessParameter(DWORD dwProcId, LPWSTR& szCmdLine, ProcessParameter parameter);

PPEB GetPebAddress(HANDLE hProcess);

typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength);

} // namespace ProcessHelper
