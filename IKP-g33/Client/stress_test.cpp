#include "stress_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void stress_test(SOCKET connectSocket, int threadIndex, int enableSleep) {
    char buffer[BUFFER_SIZE];
    char msg[] = "1,10\n";

    if (enableSleep) {
        Sleep(threadIndex * 100); // Sleep for threadIndex * 100 milliseconds
    }
    send(connectSocket, msg, (int)strlen(msg), 0);
    int iResult = recv(connectSocket, buffer, BUFFER_SIZE, 0);
    buffer[iResult] = '\0';
    printf("Thread: %d response: %s", threadIndex, buffer);
}

unsigned __stdcall stress_test_thread(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    SOCKET connectSocket = threadArgs->connectSocket;
    int threadIndex = threadArgs->threadIndex;
    int enableSleep = threadArgs->enableSleep;

    // Execute the stress_test function
    stress_test(connectSocket, threadIndex, enableSleep);
    return 0;
}

void run_stress_test(SOCKET connectSocket, int numThreads, int enableSleep) {
    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        printf("Memory allocation for threads failed.\n");
        return;
    }

    ThreadArgs threadArgs = { connectSocket };

    // Create multiple threads
    for (int i = 0; i < numThreads; i++) {
        threadArgs.connectSocket = connectSocket;
        threadArgs.threadIndex = i; // Set the thread index
        threadArgs.enableSleep = enableSleep; // Set the sleep option

        threads[i] = CreateThread(
            NULL,                      // Security attributes
            0,                         // Stack size (default)
            (LPTHREAD_START_ROUTINE)stress_test_thread,        // Thread function
            &threadArgs,               // Arguments to thread function
            0,                         // Creation flags
            NULL                       // Thread identifier
        );

        if (threads[i] == NULL) {
            printf("Failed to create thread %d\n", i);
        }
        else {
            printf("Thread: %d created.\n", i);
        }
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, 1000);

    // Close thread handles
    for (int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }

    free(threads);  // Free the dynamically allocated memory
}

bool isValidInteger(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
