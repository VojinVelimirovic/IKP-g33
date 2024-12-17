#ifndef STRESS_TEST_H
#define STRESS_TEST_H

#include <winsock2.h>

#define BUFFER_SIZE 256

typedef struct ThreadArgs {
    SOCKET connectSocket;
    int threadIndex;
    int enableSleep;
} ThreadArgs;

// Stres test koji šalje više request-ova istovremeno
void run_stress_test(SOCKET connectSocket, int numThreads, int enableSleep);

// pomocna funkcija
bool isValidInteger(const char* str);

#endif // STRESS_TEST_H
