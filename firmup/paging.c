#include "paging.h"
#include "flash64.h"

NTSTATUS
PagingVirtualToPhysical(
	HANDLE device,
	ULONGLONG cr3_raw,
	ULONGLONG va_raw,
	PULONGLONG out
	)
{
	PAGING_CR3          cr3;
	PAGING_VA_BITS      va;
	PAGING_PML4_ENTRY64 pml4e;
	PAGING_PDPTE64      pdpte;
	PAGING_PDE64        pde;
	PAGING_PTE64        pte;
	NTSTATUS            status;

	if( cr3_raw == 0 )
		return STATUS_INVALID_PARAMETER_2;

	cr3.m_full = cr3_raw;
	va.m_full  = va_raw;

	if( !NT_SUCCESS( status = FlashReadPhysicalMemory( device, PFN_TO_PAGE( cr3.m_address_of_page_directory ) + ( sizeof( pml4e ) * va.m_pml4_index ), &pml4e, sizeof( pml4e ) ) ) )
		return status;

	if( !NT_SUCCESS( status = FlashReadPhysicalMemory( device, PFN_TO_PAGE( pml4e.m_pdpt_p ) + ( sizeof( pdpte ) * va.m_pdpt_index ), &pdpte, sizeof( pdpte ) ) ) )
		return status;

	if( !NT_SUCCESS( status = FlashReadPhysicalMemory( device, PFN_TO_PAGE( pdpte.m_pd_p ) + ( sizeof( pde ) * va.m_page_dir_index ), &pde, sizeof( pde ) ) ) )
		return status;

	if( !NT_SUCCESS( status = FlashReadPhysicalMemory( device, PFN_TO_PAGE( pde.m_pt_p ) + ( sizeof( pte ) * va.m_page_table_index ), &pte, sizeof( pte ) ) ) )
		return status;

	if( pde.m_large_page )
	{
		*out = ( PFN_TO_PAGE( pde.m_pt_p ) + ( va_raw & 0x1FFFFF ) );
		return STATUS_SUCCESS;
	}
	else if( pte.m_present )
	{
		*out = ( PFN_TO_PAGE( pte.m_page_frame_number ) + ( va_raw & 0xFFF ) );
		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;
}