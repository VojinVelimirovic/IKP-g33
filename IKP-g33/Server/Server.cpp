#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#include "../HeapManager/MemorySegment.h"
#include "../HeapManager/Memory.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_PORT 27016
#define BUFFER_SIZE 256

// TCP server that use blocking sockets
int main()
{

	initialize_segments(5);

	// Allocate memory of 256 bytes
	void* allocatedMemory = allocate_memory(256);
	if (allocatedMemory != NULL) {
		printf("Memory allocated at address: %p\n", allocatedMemory);
	}
	else {
		printf("Memory allocation failed.\n");
	}

	// Clean up resources
	cleanup_segments();

	//return 0;





	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptedSocket = INVALID_SOCKET;
	int iResult;
	char dataBuffer[BUFFER_SIZE];

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}


	// Initialize serverAddress structure used by bind
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;				// IPv4 address family
	serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
	serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port


	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol

// Check if socket is successfully created
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address to socket
	iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	// Check if socket is successfully binded to address and port from sockaddr_in structure
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Set listenSocket in listening mode
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	// Struct for information about connected client
	sockaddr_in clientAddr;

	int clientAddrSize = sizeof(struct sockaddr_in);

	// Accept new connections from clients 
	acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

	// Check if accepted socket is valid 
	if (acceptedSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("\nFirst client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
	bool allocate = false;
	do
	{
		// Receive data from first client 
		iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);
		if (iResult > 0) {
			dataBuffer[iResult] = '\0';
			if (!strcmp(dataBuffer, "1")) {
				printf("Allocation request recieved!\n");
				allocate = true;
			}
			else {
				printf("Deallocation request recieved!\n");
				allocate = false;
			}
			strcpy_s(dataBuffer, "Request sent!\n");
			
			// Send message to clients using connected socket
			iResult = send(acceptedSocket, dataBuffer, (int)strlen(dataBuffer), 0);

			// Check result of send function
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				shutdown(acceptedSocket, SD_BOTH);
				closesocket(acceptedSocket);
				break;
			}
			iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);
			if (iResult > 0) {
				dataBuffer[iResult] = '\0';
				if (allocate) {
					printf("Allocating %s bytes...\n", dataBuffer);
					strcpy_s(dataBuffer, "Successfully allocated memory!\n");
					
				}
				else {
					printf("Deallocating segment on address %s...\n", dataBuffer);
					strcpy_s(dataBuffer, "Successfully deallocated memory!\n");
					
				}
				
				iResult = send(acceptedSocket, dataBuffer, (int)strlen(dataBuffer), 0);
				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					shutdown(acceptedSocket, SD_BOTH);
					closesocket(acceptedSocket);
					break;
				}
			}
		}
	} while (true);


	// Shutdown the connection since we're done
	iResult = shutdown(acceptedSocket, SD_BOTH);

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket);
		WSACleanup();
		return 1;
	}

	// Shutdown the connection since we're done

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket);
		WSACleanup();
		return 1;
	}
	//Close listen and accepted sockets
	closesocket(listenSocket);
	closesocket(acceptedSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}