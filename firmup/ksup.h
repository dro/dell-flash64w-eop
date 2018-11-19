#pragma once

PVOID
KsupGetSystemProcess(
	HANDLE app_handle
	);

NTSTATUS
KsupLookupProcessById(
	HANDLE app_handle,
	ULONGLONG pid,
	PVOID *process_out
	);

NTSTATUS
KsupReadProcessVirtualMemory(
	HANDLE app_handle,
	PVOID process,
	ULONGLONG va,
	PVOID out,
	DWORD size
	);

NTSTATUS
KsupWriteProcessVirtualMemory(
	HANDLE app_handle,
	PVOID process,
	ULONGLONG va,
	PVOID in,
	DWORD size
	);