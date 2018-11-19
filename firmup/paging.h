#pragma once

#include "runtime.h"

#define PFN_TO_PAGE( pfn ) ( pfn << 12 )

NTSTATUS
PagingVirtualToPhysical(
	HANDLE device,
	ULONGLONG cr3_raw,
	ULONGLONG va_raw,
	PULONGLONG out
	);

typedef union _PAGING_VA_BITS
{
	struct
	{
		/* 00 */ ULONGLONG m_page_offset : 12;
		/* 12 */ ULONGLONG m_page_table_index : 9;
		/* 21 */ ULONGLONG m_page_dir_index : 9;
		/* 30 */ ULONGLONG m_pdpt_index : 9;
		/* 39 */ ULONGLONG m_pml4_index : 9;
		/* 48 */ ULONGLONG m_reserved_bits : 16;
	};

	ULONGLONG m_full;
} PAGING_VA_BITS, *PPAGING_VA_BITS;

typedef union _PAGING_CR3
{
	struct
	{
		ULONGLONG m_reserved_1 : 3;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_reserved_2 : 7;
		ULONGLONG m_address_of_page_directory : 36;
	};

	ULONGLONG m_full;
} PAGING_CR3, *PPAGING_CR3;

typedef union _PAGING_PTE64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_dirty : 1;
		ULONGLONG m_pat : 1;
		ULONGLONG m_global : 1;
		ULONGLONG m_ignored_1 : 3;
		ULONGLONG m_page_frame_number : 36;
		ULONGLONG m_reserved_1 : 4;
		ULONGLONG m_ignored_2 : 7;
		ULONGLONG m_protection_key : 4;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_PTE64, *PPAGING_PTE64;

typedef union _PAGING_2MB_PDE64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_dirty : 1;
		ULONGLONG m_large_page : 1;
		ULONGLONG m_global : 1;
		ULONGLONG m_ignored_1 : 3;
		ULONGLONG m_pat : 1;
		ULONGLONG m_reserved_1 : 17;
		ULONGLONG m_page_frame_number : 18;
		ULONGLONG m_reserved_2 : 4;
		ULONGLONG m_ignored_2 : 7;
		ULONGLONG m_protection_key : 4;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_2MB_PDE64, *PPAGING_2MB_PDE64;

typedef union _PAGING_PDE64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_reserved_1 : 1;
		ULONGLONG m_large_page : 1;
		ULONGLONG m_ignored_1 : 4;
		ULONGLONG m_pt_p : 40;
		ULONGLONG m_ignored_2 : 11;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_PDE64, *PPAGING_PDE64;

typedef union _PAGING_1GB_PDPTE64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_dirty : 1;
		ULONGLONG m_large_page : 1;
		ULONGLONG m_global : 1;
		ULONGLONG m_ignored_1 : 3;
		ULONGLONG m_pat : 1;
		ULONGLONG m_reserved_1 : 17;
		ULONGLONG m_page_frame_number : 18;
		ULONGLONG m_reserved_2 : 4;
		ULONGLONG m_ignored_2 : 7;
		ULONGLONG m_protection_key : 4;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_1GB_PDPTE64, *PPAGING_1GB_PDPTE64;

typedef union _PAGING_PDPTE64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_reserved_1 : 1;
		ULONGLONG m_large_page : 1;
		ULONGLONG m_ignored_1 : 4;
		ULONGLONG m_pd_p : 40;
		ULONGLONG m_ignored_2 : 11;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_PDPTE64, *PPAGING_PDPTE64;

typedef union _PAGING_PML4_ENTRY64
{
	struct
	{
		ULONGLONG m_present : 1;
		ULONGLONG m_write : 1;
		ULONGLONG m_supervisor : 1;
		ULONGLONG m_page_level_write_through : 1;
		ULONGLONG m_page_level_cache_disable : 1;
		ULONGLONG m_accessed : 1;
		ULONGLONG m_reserved_1 : 1;
		ULONGLONG m_must_be_zero : 1;
		ULONGLONG m_ignored_1 : 4;
		ULONGLONG m_pdpt_p : 40;
		ULONGLONG m_ignored_2 : 11;
		ULONGLONG m_execute_disable : 1;
	};

	ULONGLONG m_full;
} PAGING_PML4_ENTRY64, *PPAGING_PML4_ENTRY64;