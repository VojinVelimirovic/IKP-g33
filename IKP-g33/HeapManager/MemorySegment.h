#pragma once
#include <windows.h>

#define SEGMENT_SIZE 64    // Velicina segmenta, moze da se poveca

// Memorija je organizovana u segmente predefinisane velicine
typedef struct MemorySegment {
	long address;
	bool isFree;
	HANDLE mutex;		// zakljucati segment dok se upisuje u njega - kriticka sekcija
}TMemorySegment;


// Klijentske metode barataju sa BLOKOVIMA
typedef struct Block {
	long start_address;
	int size;				// treba ili size 
	int segments_taken;		// ili segments_taken
}TBlock;