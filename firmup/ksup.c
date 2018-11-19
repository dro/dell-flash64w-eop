#include "runtime.h"
#include "kpsb.h"
#include "paging.h"
#include "bootstrap.h"
#include "ksup.h"
#include "app.h"

// Internal use only!
typedef struct _APP_DATA
{
	UINT32          m_hdr;
	PKERNEL_CONTEXT m_ctx;
	HANDLE          m_drv;
} APP_DATA, *PAPP_DATA;

PVOID
KsupGetSystemProcess(
	HANDLE app_handle
	)
{
	PAPP_DATA  app;
	ULONGLONG  kernel_base;
	ULONGLONG  system_process;

	app = ( PAPP_DATA )app_handle;

	if( app->m_hdr != APP_DATA_MAGIC )
		return NULL;

	kernel_base    = ( app->m_ctx->m_kentry - app->m_ctx->m_ep_offset );
	system_process = ( kernel_base + app->m_ctx->m_sysproc_offset );

	if( !NT_SUCCESS( FlashReadVirtualMemory( app->m_drv, system_process, &system_process, sizeof( system_process ) ) ) )
		return NULL;

	return ( PVOID )system_process;
}

NTSTATUS
KsupLookupProcessById(
	HANDLE app_handle,
	ULONGLONG pid,
	PVOID *process_out
	)
{
	PAPP_DATA  app;
	ULONGLONG  current_link;
	ULONGLONG  end_link;
	ULONGLONG  current_process;
	ULONGLONG  system_process;
	ULONGLONG  current_pid;
	NTSTATUS   status;

	app = ( PAPP_DATA )app_handle;

	if( app->m_hdr != APP_DATA_MAGIC )
		return STATUS_INVALID_HANDLE;

	system_process = ( ULONGLONG )KsupGetSystemProcess( app_handle );

	if( system_process == 0 )
		return STATUS_UNSUCCESSFUL;

	current_process = system_process;
	current_link    = ( current_process + app->m_ctx->m_apl_offset );
	end_link        = ( current_link + 8 );

	if( !NT_SUCCESS( status = FlashReadVirtualMemory( app->m_drv, end_link, &end_link, sizeof( end_link ) ) ) )
		return status;

	do
	{
		current_process = ( current_link - app->m_ctx->m_apl_offset );

		if( NT_SUCCESS( status = FlashReadVirtualMemory( app->m_drv,
														 current_process + app->m_ctx->m_pid_offset,
														 &current_pid,
														 sizeof( current_pid ) ) ) )
		{
			if( current_pid == pid )
			{
				*process_out = ( PVOID )current_process;
				return STATUS_SUCCESS;
			}
		}

		if( !NT_SUCCESS( status = FlashReadVirtualMemory( app->m_drv, current_link, &current_link, sizeof( current_link ) ) ) )
			return status;

	} while( current_link != end_link );

	return STATUS_NOT_FOUND;
}

NTSTATUS
KsupReadProcessVirtualMemory(
	HANDLE app_handle,
	PVOID process,
	ULONGLONG va,
	PVOID out,
	DWORD size
	)
{
	PAPP_DATA app;
	NTSTATUS  status;
	ULONGLONG dirbase;
	ULONGLONG old_dirbase;

	app = ( PAPP_DATA )app_handle;

	if( app->m_hdr != APP_DATA_MAGIC )
		return STATUS_INVALID_HANDLE;

	if( !NT_SUCCESS( status = FlashReadVirtualMemory( app->m_drv, ( ( ULONGLONG )process + 0x28 ), &dirbase, sizeof( dirbase ) ) ) )
		return status;

	FlashSetUsedCr3Value( app->m_drv, dirbase, &old_dirbase );
	status = FlashReadVirtualMemory( app->m_drv, va, out, size );
	FlashSetUsedCr3Value( app->m_drv, old_dirbase, NULL );

	return status;
}

NTSTATUS
KsupWriteProcessVirtualMemory(
	HANDLE app_handle,
	PVOID process,
	ULONGLONG va,
	PVOID in,
	DWORD size
	)
{
	PAPP_DATA app;
	NTSTATUS  status;
	ULONGLONG dirbase;
	ULONGLONG old_dirbase;

	app = ( PAPP_DATA )app_handle;

	if( app->m_hdr != APP_DATA_MAGIC )
		return STATUS_INVALID_HANDLE;

	if( !NT_SUCCESS( status = FlashReadVirtualMemory( app->m_drv, ( ( ULONGLONG )process + 0x28 ), &dirbase, sizeof( dirbase ) ) ) )
		return status;

	FlashSetUsedCr3Value( app->m_drv, dirbase, &old_dirbase );
	status = FlashWriteVirtualMemory( app->m_drv, va, in, size );
	FlashSetUsedCr3Value( app->m_drv, old_dirbase, NULL );

	return status;
}