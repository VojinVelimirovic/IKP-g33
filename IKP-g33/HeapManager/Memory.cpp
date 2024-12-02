#include <stdio.h>
#include <stdlib.h>
#include "MemorySegment.h"

//            pokazivac na niz segmenata, ukupan broj segmenata, velicina bloka
long firstFit(TMemorySegment* segments, int segmentCount, int blockSize) {
    // Racunanje broja segmenata potrebnih za blok, -1 je potreban da se ne desi overestimacija
    int requiredSegments = (blockSize + SEGMENT_SIZE - 1) / SEGMENT_SIZE;

    int freeCount = 0;  // brojac kontinualnih slobodnih segmenata
    int startIndex = -1; // index na koji blok treba da se ubaci

    // Prolazimo kroz niz segmenata
    for (int i = 0; i < segmentCount; i++) {
        if (segments[i].isFree) {
            if (freeCount == 0) {
                startIndex = i; // index pocetka novog bloka
            }
            freeCount++;
            if (freeCount == requiredSegments) {
                // Ako je to to vracamo adresu prvog segmenta u bloku
                return segments[startIndex].address;
            }
        }
        else {
            freeCount = 0; // Reset brojaca ako se naidje na zauzet element, jer lomi kontinualnost
        }
    }

    // Ako nije nadjen blok vracamo null
    return (long)NULL;
}

