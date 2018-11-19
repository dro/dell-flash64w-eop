#include "runtime.h"

#include <intrin.h>

PVOID
RtAllocateHeap(
	SIZE_T size
	)
/*++

Routine Description:

	Heap memory manager allocation implementation.
	Function will attempt to acquire a heap allocation until it receives one.

Arguments:

	size - Size of heap allocation.

Return Value:

	Pointer to heap allocation.
--*/
{
	PPEB   peb;
	PVOID  alloc;
	SIZE_T alloc_size;

	peb        = NtCurrentPeb( );
	alloc_size = ( ( size != 0 ) ? size : 1 );

	do
	{
		alloc = RtlAllocateHeap( peb->ProcessHeap, 0, alloc_size );
	} while( alloc == NULL );

	return alloc;
}

PVOID
RtAllocateHeapZeroed(
	SIZE_T size
	)
/*++

Routine Description:

	Zero initialized RtAllocateHeap wrapper.

Arguments:

	size - Size of heap allocation.

Return Value:

	Pointer to heap allocation.
--*/
{
	PVOID allocation;

	allocation = RtAllocateHeap( size );

	if( allocation != NULL )
		RtSetMemory( allocation, 0, size );

	return allocation;
}

PVOID
RtReAllocateHeap(
	PVOID  base,
	SIZE_T size
	)
/*++

Routine Description:

	Heap memory manager re-allocation implementation.

Arguments:

	base - Old heap allocation to be resized.
	size - Size of new heap allocation.

Return Value:

	Resized heap allocation or NULL on failure.
--*/
{
	PPEB peb;

	peb = NtCurrentPeb( );

	return RtlReAllocateHeap( peb->ProcessHeap, 0, base, size );
}

BOOLEAN
RtFreeHeap(
	PVOID alloc
	)
/*++


Routine Description:

	Heap memory manager free implementation.

Arguments:

	alloc - Base of heap allocation to free.

Return Value:

	Boolean success/failure.
--*/
{
	PPEB peb;

	peb = NtCurrentPeb( );

	return RtlFreeHeap( peb->ProcessHeap, 0, alloc );
}

SIZE_T
RtStringLen(
	PCSTR string
	)
{
	SIZE_T len;

	for( len = 0; string[ len ] != '\0'; ++len );

	return len;
}

VOID
RtStringCopyLen(
	PSTR  dest,
	PCSTR from,
	SIZE_T len
	)
{
	RtCopyMemory( ( PVOID ) dest, ( PVOID ) from, len );
	dest[ len ] = '\0';
}

VOID
RtStringCopy(
	PSTR  dest,
	PCSTR from
	)
{
	RtStringCopyLen( dest, from, RtStringLen( from ) );
}

PSTR
RtStringCat(
	PSTR dest,
	PCSTR from
	)
{
	PSTR str;

	// locate terminator
	for( str = dest; *str++; ) { ; }

	// concat
	for( --str; ( *str++ = *from++ ); ) { ; }

	return dest;
}

VOID
RtCopyMemory(
	PVOID dest,
	PVOID from,
	SIZE_T len
	)
{
	__movsb( ( PUCHAR )dest, ( const PUCHAR )from, len );
}

VOID
RtSetMemory(
	PVOID dest,
	UINT8 byte,
	SIZE_T len
	)
{
	__stosb( ( PUCHAR )dest, byte, len );
}

PCSTR
RtStringSubchar(
	PCSTR string,
	CHAR ch
	)
{
	PSTR search;

	for( search = ( PSTR )string; *search != '\0'; ++search )
	{
		if( *search == ch )
			return search;
	}

	return NULL;
}

PCSTR
RtStringSubstring(
	PCSTR string,
	PCSTR string2
	)
{
	PSTR    search, s1, s2;

	search = ( PSTR )string;

	while( *search )
	{
		s1 = search;
		s2 = ( PSTR )string2;

		while( *s1 && *s2 )
		{
			if( *s1 != *s2 )
				break;

			++s1, ++s2;
		}

		if( *s2 == '\0' )
			return search;

		++search;
	}

	return NULL;
}

CHAR
RtCharToLower(
	CHAR ch
	)
{
	return( ( ch >= 'A' && ch <= 'Z' ) ? ch + ( 'a' - 'A' ) : ch );
}

BOOLEAN
RtCharIsNumeric(
	CHAR ch
	)
{
	return( ( ch >= '0' ) && ( ch <= '9' ) );
}

VOID
RtStringToLowerLength(
	PSTR string,
	SIZE_T len
	)
{
	SIZE_T i;

	for( i = 0; i < len; ++i )
		string[ i ] = RtCharToLower( string[ i ] );
}

VOID
RtStringToLower(
	PSTR string
	)
{
	PSTR i;

	for( i = string; *i != '\0'; ++i )
		*i = RtCharToLower( *i );
}