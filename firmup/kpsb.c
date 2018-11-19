#include "kpsb.h"

BOOLEAN
KpsbIsValid(
	PPROCESSOR_START_BLOCK block
	)
{
	if( block->Jmp.OpCode != 0xE9 )
		return FALSE;

	if( block->Jmp.Offset != 0x0006 )
		return FALSE;

	return TRUE;
}

BOOLEAN
KpsbScan(
	HANDLE device,
	PPROCESSOR_START_BLOCK *out
	)
{
	ULONGLONG              current_pa;
	PPROCESSOR_START_BLOCK stub;

	current_pa = 0x1000;
	stub       = RtAllocateHeapZeroed( 0x1000 );

	while( current_pa < 0x100000 )
	{
		RtSetMemory( stub, 0, 0x1000 );

		if( NT_SUCCESS( FlashReadPhysicalMemory( device, current_pa, stub, 0x1000 ) ) )
		{
			if( KpsbIsValid( stub ) )
			{
				*out = stub;
				return TRUE;
			}
		}

		current_pa += 0x1000;
	}

	return FALSE;
}