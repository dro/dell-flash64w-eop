#include "runtime.h"
#include "kpsb.h"
#include "paging.h"
#include "bootstrap.h"
#include "app.h"
#include "flash64.h"

// Internal use only!
typedef struct _APP_DATA
{
	UINT32          m_hdr;
	PKERNEL_CONTEXT m_ctx;
	HANDLE          m_drv;
} APP_DATA, *PAPP_DATA;

NTSTATUS
AppCreate(
	PHANDLE app_handle
	)
{
	PAPP_DATA              app;
	HANDLE                 driver;
	NTSTATUS               status;
	PPROCESSOR_START_BLOCK stub;
	PKERNEL_CONTEXT        kctx;

	if( !( app = RtAllocateHeapZeroed( sizeof( APP_DATA ) ) ) )
		return STATUS_INSUFFICIENT_RESOURCES;

	app->m_drv = INVALID_HANDLE_VALUE;
	app->m_hdr = APP_DATA_MAGIC;

	if( !( kctx = RtAllocateHeapZeroed( sizeof( KERNEL_CONTEXT ) ) ) )
		return STATUS_INSUFFICIENT_RESOURCES;

	if( !NT_SUCCESS( status = BsSetupKernelContext( kctx ) ) )
		return status;

	if( !NT_SUCCESS( status = FlashOpenDriver( &driver ) ) )
		return status;

	if( !KpsbScan( driver, &stub ) )
		return STATUS_UNSUCCESSFUL;

	FlashSetUsedCr3Value( driver, stub->ProcessorState.SpecialRegisters.Cr3, NULL );

	kctx->m_kentry = stub->ProcessorState.ContextFrame.Rip;
	app->m_drv     = driver;
	app->m_ctx     = kctx;

	*app_handle = ( HANDLE )app;

	return STATUS_SUCCESS;
}

VOID
AppClose(
	HANDLE app_handle
	)
{
	PAPP_DATA app;

	app = ( PAPP_DATA )app_handle;

	if( app->m_hdr != APP_DATA_MAGIC )
		return;

	if( app->m_ctx != NULL )
		RtFreeHeap( app->m_ctx );

	FlashCloseDriver( app->m_drv );

	RtFreeHeap( app );
}