#pragma once

#include "runtime.h"
#include "startblock.h"
#include "flash64.h"

BOOLEAN
KpsbScan(
	HANDLE device,
	PPROCESSOR_START_BLOCK *out
	);

BOOLEAN
KpsbIsValid(
	PPROCESSOR_START_BLOCK block
	);