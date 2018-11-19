#pragma once

#include "runtime.h"

typedef struct _KERNEL_CONTEXT
{
	ULONG32   m_pid_offset;
	ULONG32   m_apl_offset;
	ULONG32   m_sysproc_offset;
	ULONG32   m_ep_offset;
	ULONG32   m_peb_offset;
	ULONGLONG m_kentry;
} KERNEL_CONTEXT, *PKERNEL_CONTEXT;

NTSTATUS
BsSetupKernelContext(
	PKERNEL_CONTEXT ctx
	);