#pragma once

#define APP_DATA_MAGIC 'mDpA'

NTSTATUS
AppCreate(
	PHANDLE app_handle
	);

VOID
AppClose(
	HANDLE app_handle
	);