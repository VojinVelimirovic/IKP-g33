#pragma once
#include <windows.h>

struct MemorySegment {
	long address;
	int size;
	bool status;
	HANDLE mutex;
};