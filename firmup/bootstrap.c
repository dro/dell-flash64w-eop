#include "bootstrap.h"
#include "hde64.h"

NTSTATUS
BspGetRoutineInsDisp32(
	PVOID base,
	PSTR routine_name,
	PULONG32 out
	)
{
	ANSI_STRING ansi_name;
	PVOID       routine_addr;
	NTSTATUS    status;
	hde64s      ins;

	RtlInitAnsiString( &ansi_name, routine_name );

	if( !NT_SUCCESS( status = LdrGetProcedureAddress( base, &ansi_name, 0, &routine_addr ) ) )
		return status;

	if( hde64_disasm( routine_addr, &ins ) == 0 )
		return STATUS_INVALID_ADDRESS;

	if( !( ins.flags & F_DISP32 ) )
		return STATUS_INVALID_ADDRESS;

	*out = ins.disp.disp32;

	return STATUS_SUCCESS;
}

NTSTATUS
BspFindActiveProcessLinksOffset(
	PVOID base,
	PULONG32 out
	)
{
	ANSI_STRING export_name;
	PVOID       PsGetProcessId;
	NTSTATUS    status;
	hde64s      ins;

	// ActiveProcessLinks field of EPROCESS lies +8 of UniqueProcessId
	RtlInitAnsiString( &export_name, "PsGetProcessId" );

	if( !NT_SUCCESS( status = LdrGetProcedureAddress( base, &export_name, 0, &PsGetProcessId ) ) )
		return status;

	if( hde64_disasm( PsGetProcessId, &ins ) == 0 )
		return STATUS_INVALID_ADDRESS;

	if( !( ins.flags & F_DISP32 ) )
		return STATUS_INVALID_ADDRESS;

	*out = ( ins.disp.disp32 + 8 );

	return STATUS_SUCCESS;
}

NTSTATUS
BspFindPebOffset(
	PVOID base,
	PULONG32 out
	)
{
	ANSI_STRING export_name;
	PVOID       PsGetProcessPeb;
	NTSTATUS    status;
	hde64s      ins;

	RtlInitAnsiString( &export_name, "PsGetProcessPeb" );

	if( !NT_SUCCESS( status = LdrGetProcedureAddress( base, &export_name, 0, &PsGetProcessPeb ) ) )
		return status;

	if( hde64_disasm( PsGetProcessPeb, &ins ) == 0 )
		return STATUS_INVALID_ADDRESS;

	if( !( ins.flags & F_DISP32 ) )
		return STATUS_INVALID_ADDRESS;

	*out = ins.disp.disp32;

	return STATUS_SUCCESS;
}

NTSTATUS
BspFindInitialSystemProcessOffset(
	PVOID base,
	PULONG32 out
	)
{
	ANSI_STRING export_name;
	PVOID       PsInitialSystemProcess;
	NTSTATUS    status;

	RtlInitAnsiString( &export_name, "PsInitialSystemProcess" );

	if( !NT_SUCCESS( status = LdrGetProcedureAddress( base, &export_name, 0, &PsInitialSystemProcess ) ) )
		return status;

	*out = ( ULONG32 )( ( PUCHAR )PsInitialSystemProcess - ( PUCHAR )base );

	return STATUS_SUCCESS;
}

NTSTATUS
BspFindEPOffset(
	PVOID base,
	PULONG32 out
	)
{
	PIMAGE_NT_HEADERS hdr;

	if( !( hdr = RtlImageNtHeader( base ) ) )
		return STATUS_INVALID_IMAGE_FORMAT;

	*out = hdr->OptionalHeader.AddressOfEntryPoint;

	return STATUS_SUCCESS;
}

NTSTATUS
BsSetupKernelContext(
	PKERNEL_CONTEXT ctx
	)
{
	PVOID          ntoskrnl;
	UNICODE_STRING name;
	ULONG		   flags;
	NTSTATUS       status;

	RtlInitUnicodeString( &name, L"ntoskrnl.exe" );

	if( !NT_SUCCESS( status = LdrLoadDll( NULL, &flags, &name, &ntoskrnl ) ) )
		return status;

	if( !NT_SUCCESS( status = BspFindInitialSystemProcessOffset( ntoskrnl, &ctx->m_sysproc_offset ) ) )
		return status;

	if( !NT_SUCCESS( status = BspFindEPOffset( ntoskrnl, &ctx->m_ep_offset ) ) )
		return status;

	if( !NT_SUCCESS( status = BspGetRoutineInsDisp32( ntoskrnl, "PsGetProcessId", &ctx->m_pid_offset ) ) )
		return status;

	if( !NT_SUCCESS( status = BspGetRoutineInsDisp32( ntoskrnl, "PsGetProcessPeb", &ctx->m_peb_offset ) ) )
		return status;

	ctx->m_apl_offset = ( ctx->m_pid_offset + 8 );

	LdrUnloadDll( ntoskrnl );

	return STATUS_SUCCESS;
}
