#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 27016
#define BUFFER_SIZE 256

// TCP client that use blocking sockets
int main()
{
	// Socket used to communicate with server
	SOCKET connectSocket = INVALID_SOCKET;

	// Variable used to store function return value
	int iResult;

	// Buffer we will use to store message
	char buffer[BUFFER_SIZE];

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	// create a socket
	connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;								// IPv4 protocol
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);	// ip address of server
	serverAddress.sin_port = htons(SERVER_PORT);					// server port

	// Connect to server specified in serverAddress and socket connectSocket
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	
	while (strcmp(buffer, "x") != 0)
	{
		fseek(stdin, 0, SEEK_END);
		int answer;
		printf("1. Allocate memory\n2. Free memory\nx. Quit\n");
		gets_s(buffer, BUFFER_SIZE);
		if (strcmp(buffer, "x") == 0) {
			break;
		}
		if (strcmp(buffer, "1") && strcmp(buffer, "2")){
			printf("Invalid option. Please try again.\n");
			continue;
		}
		answer = atoi(buffer);
		iResult = send(connectSocket, buffer, (int)strlen(buffer), 0);
		fseek(stdin, 0, SEEK_END);
		fflush(stdin);

		iResult = recv(connectSocket, buffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			buffer[iResult] = '\0';
			printf(buffer);
			if (answer == 1) {
				printf("Enter size of memory you want to allocate:\n");
				gets_s(buffer, BUFFER_SIZE);
			}
			else if (answer == 2) {
				printf("Enter the address that you want to free:\n");
				gets_s(buffer, BUFFER_SIZE);
			}

			iResult = send(connectSocket, buffer, (int)strlen(buffer), 0);

			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}
		}
		else {
			continue;
		}
		fseek(stdin, 0, SEEK_END);
		fflush(stdin);

		iResult = recv(connectSocket, buffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			buffer[iResult] = '\0';
			printf(buffer);
		}
		else
		{
			printf("received failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
	}
	


	// Shutdown the connection since we're done
	iResult = shutdown(connectSocket, SD_BOTH);

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}


	// Close connected socket
	closesocket(connectSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}