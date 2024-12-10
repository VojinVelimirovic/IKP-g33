#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "../HeapManager/MemorySegment.h"
#include "../HeapManager/Memory.h"
#include "../HeapManager/Queue.h"

#define SERVER_PORT 27016
#define BUFFER_SIZE 256

// Atomic flag for server shutdown
volatile LONG serverRunning = 1;


// Function for worker threads to process requests
DWORD WINAPI processRequest(LPVOID param) {
	Queue* queue = (Queue*)param;
	DWORD threadId = GetCurrentThreadId(); // Get the current thread ID

	while (serverRunning /*|| queue->front != NULL*/) {
		Request request;
		if (dequeue(queue, &request)) {
			char responseBuffer[BUFFER_SIZE]; // Buffer to send responses back to the client

			// Log which thread is processing the request
			if (request.isAllocate) {
				printf("Thread ID %lu: Processing allocation request for %d bytes.\n", threadId, request.value);
				int allocatedBlockAddress = (int)allocate_memory(request.value);
				if (allocatedBlockAddress != -1) {
					printf("SUCCESS: Allocated %d bytes at address: %d\n\n", request.value, allocatedBlockAddress);
					snprintf(responseBuffer, BUFFER_SIZE, "SUCCESS: Allocated %d bytes at address: %d\n\n", request.value, allocatedBlockAddress);
				}
				else {
					printf("ERROR: Memory allocation failed for %d bytes.\n", request.value);
					strcpy_s(responseBuffer, "ERROR: Memory allocation failed.");
				}
			}
			else {
				printf("Thread ID %lu: Processing deallocation request for address: %d.\n", threadId, request.value);
				free_memory((void*)request.value);
				if (free_memory_error == 0) {
					printf("SUCCESS: Memory freed at address: %d\n", request.value);
					snprintf(responseBuffer, BUFFER_SIZE, "SUCCESS: Memory freed at address: %d\n\n", request.value);
				}
				else {
					printf("ERROR: Failed to free memory at address: %d.\n", request.value);
					snprintf(responseBuffer, BUFFER_SIZE, "ERROR: Failed to free memory at address: %d.\n\n", request.value);
				}
			}

			drawMemorySegments();

			// Send the response back to the client
			if (send(request.clientSocket, responseBuffer, (int)strlen(responseBuffer), 0) == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
			}
		}
	}
	return 0;
}


// TCP server that use blocking sockets
int main()
{
	initializeMemory(5);

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptedSocket = INVALID_SOCKET;
	int iResult;
	char dataBuffer[BUFFER_SIZE];

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(SERVER_PORT);

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(struct sockaddr_in);

	// Initialize the queue
	Queue requestQueue;
	initializeQueue(&requestQueue);

	// Create worker threads
	const int threadPoolSize = 4; // Adjust as needed
	HANDLE* threadPool = (HANDLE*)malloc(threadPoolSize * sizeof(HANDLE));
	for (int i = 0; i < threadPoolSize; ++i) {
		threadPool[i] = CreateThread(NULL, 0, processRequest, (LPVOID)&requestQueue, 0, NULL);
	}

	while (serverRunning) {
		acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (acceptedSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			break;
		}

		printf("\nClient request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		while (true) {
			iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);
			if (iResult > 0) {
				dataBuffer[iResult] = '\0';
				int command_code, value;
				char* token;

				// Extract the first token
				token = strtok(dataBuffer, ",");
				if (token != NULL) {
					command_code = atoi(token);  // Convert to integer
				}
				else {
					fprintf(stderr, "Error: Missing first parameter.\n");
					return 1;
				}

				// Extract the second token
				token = strtok(NULL, ",");
				if (token != NULL) {
					value = atoi(token);  // Convert to integer
				}
				else {
					fprintf(stderr, "Error: Missing second parameter.\n");
					return 1;
				}

				Request newRequest;
				newRequest.clientSocket = acceptedSocket;

				if (command_code == 1) {
					printf("\nAllocation request received.\n");
					newRequest.isAllocate = true;
				}
				else if (command_code == 2) {
					printf("\nDeallocation request received.\n");
					newRequest.isAllocate = false;
				}
				else {
					printf("Error");
				}

				newRequest.value = value;

				// Add request to queue
				enqueue(&requestQueue, newRequest);
				WakeConditionVariable(&requestQueue.notEmpty);

			}
		}
		shutdown(acceptedSocket, SD_BOTH);
		closesocket(acceptedSocket);
	}

	// Clean up
	serverRunning = 0;
	for (int i = 0; i < threadPoolSize; i++) {
		WakeConditionVariable(&requestQueue.notEmpty); // Wake up any sleeping threads
		WaitForSingleObject(threadPool[i], INFINITE); // Wait for threads to finish
		CloseHandle(threadPool[i]); // Close thread handle
	}
	free(threadPool);
	freeQueue(&requestQueue);
	closesocket(listenSocket);
	WSACleanup();
	cleanup_segments();

	return 0;
}