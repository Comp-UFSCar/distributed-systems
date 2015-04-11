#include <stdlib.h>
#include <stdio.h>
#include "my_socket.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "AppObjects.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <signal.h>

#define DEFAULT_PORT "5000"

extern DWORD WINAPI thread_Send(LPVOID lpParameter);
extern DWORD WINAPI thread_Receive(LPVOID lpParameter);
extern void cleanUp();

int main(int argc, char *argv[]);

void intHandler(int dummy) {
	cleanUp();
	printf("\nBye\n");
}

SOCKET openOurVerySocket(SocketParams *params) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo
		*result = NULL,
		*ptr = NULL,
		hints;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return NULL;
	}

	ZeroMemory(&hints, sizeof(hints));
	// Internet address family is unspecified so that either an 
	// IPv6 or IPv4 address can be returned. The application requests 
	// the socket type to be a stream socket for the TCP protocol
	hints.ai_family = params->family;
	hints.ai_socktype = params->socktype;
	hints.ai_protocol = params->protocol;

	/* Resolve the server address and port */
	// getaddrinfo function request the IP address for the server name and 
	// the TCP port on the server that the client will connect
	iResult = getaddrinfo(params->ip, params->port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return NULL;
	}
	/* Attempt to connect to an address until one succeeds  */
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		/* Create a SOCKET for connecting to server */
		ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return NULL;
		}

		/* Connect to server - For a client to communicate on a network, it must connect
		to a server. Connect function request the created socket and the sockaddr
		structure as parameters */
		iResult = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ClientSocket);
			ClientSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	return ClientSocket;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, intHandler);

	SocketParams params;
	strcpy(params.port, DEFAULT_PORT);

	if (argc != 2) {
		printf("usage: client [server ip address] \n");
		exit(1);
	}

	params.ip = argv[1];
	params.family = AF_UNSPEC;
	params.socktype = SOCK_STREAM; 
	params.protocol = IPPROTO_TCP;
	
	ClientSocket = openOurVerySocket(&params);

	if (ClientSocket == INVALID_SOCKET) {
		printf("Error: could not establish connection with the server. Try again later.\n");
		getchar();
		exit(1);
	}
	
	ClienteSend = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Send, &ClientSocket, 0, 0);
	ClienteReceive = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Receive, &ClientSocket, 0, 0);

	WaitForSingleObject(ClienteSend, INFINITE);
	WaitForSingleObject(ClienteReceive, INFINITE);

	cleanUp();
	
	getchar();
	return 0;
}
