#pragma once

#define PHNT_VERSION PHNT_THRESHOLD

#include <phnt_windows.h>
#include <phnt.h>

PVOID
RtAllocateHeap(
	SIZE_T size
	);

PVOID
RtReAllocateHeap(
	PVOID  base,
	SIZE_T size
	);

PVOID
RtAllocateHeapZeroed(
	SIZE_T size
	);

BOOLEAN
RtFreeHeap(
	PVOID base
	);

SIZE_T
RtStringLen(
	PCSTR string
	);

VOID
RtStringCopyLen(
	PSTR  dest,
	PCSTR from,
	SIZE_T len
	);

VOID
RtStringCopy(
	PSTR  dest,
	PCSTR from
	);

PSTR
RtStringCat(
	PSTR dest,
	PCSTR from
	);

VOID
RtCopyMemory(
	PVOID dest,
	PVOID from,
	SIZE_T len
	);

VOID
RtSetMemory(
	PVOID dest,
	UINT8 byte,
	SIZE_T len
	);

PCSTR
RtStringSubchar(
	PCSTR string,
	CHAR ch
	);

CHAR
RtCharToLower(
	CHAR ch
	);

VOID
RtStringToLower(
	PSTR string
	);

VOID
RtStringToLowerLength(
	PSTR string,
	SIZE_T len
	);

PCSTR
RtStringSubstring(
	PCSTR string,
	PCSTR string2
	);

BOOLEAN
RtCharIsNumeric(
	CHAR ch
	);
