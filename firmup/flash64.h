#pragma once

#include "runtime.h"

#define DBUTIL_SYMLINK_PATH ( L"\\??\\DBUtil_2_3" )
#define DBUTIL_IOCTL_READPA  0x9B0C1F40
#define DBUTIL_IOCTL_WRITEPA ( DBUTIL_IOCTL_READPA + 4 )

#define FLASH_DRIVER_CONTEXT_MAGIC 'mCdF'

typedef struct _DELL_PACKET_ALLOCATEPA
{
	ULONGLONG m_magic;
	DWORD     m_alloc_size;
	DWORD     m_align_pad01;
	ULONGLONG m_lowest_pa;   // encoded
	ULONGLONG m_highest_pa;  // encoded

	// out by driver
	ULONGLONG m_returned_va;
	ULONGLONG m_returned_pa; // encoded
} DELL_PACKET_ALLOCATEPA, *PDELL_PACKET_ALLOCATEPA;

//typedef struct _DELL_PACKET_READWRITEPA
//{
//	ULONGLONG m_magic;
//	DWORD     m_size;
//	DWORD     m_align_pad01;
//	ULONGLONG m_pa;     // encoded
//	UINT8     m_data[]; // in/out data
//} DELL_PACKET_READWRITEPA, *PDELL_PACKET_READWRITEPA;

typedef struct _DELL_PACKET_READWRITEPA
{
	ULONGLONG m_magic;
	ULONGLONG m_pa;     // encoded
	UINT8     m_data[]; // in/out data
} DELL_PACKET_READWRITEPA, *PDELL_PACKET_READWRITEPA;

NTSTATUS
FlashOpenDriver(
	PHANDLE handle_out
	);

NTSTATUS
FlashCloseDriver(
	HANDLE driver
	);

NTSTATUS
FlashReadPhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID out,
	DWORD size
	);

NTSTATUS
FlashWritePhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID in,
	DWORD size
	);

NTSTATUS
FlashReadWritePhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID out,
	DWORD size,
	BOOLEAN write
	);

NTSTATUS
FlashSetUsedCr3Value(
	HANDLE driver,
	ULONGLONG value,
	PULONGLONG optional_old
	);

NTSTATUS
FlashReadVirtualMemory(
	HANDLE driver,
	ULONGLONG va,
	PVOID out,
	DWORD size
	);

NTSTATUS
FlashWriteVirtualMemory(
	HANDLE driver,
	ULONGLONG va,
	PVOID in,
	DWORD size
	);