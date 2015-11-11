/* Servidor.cpp - Generated by Visual Multi-Thread Win32 */

// WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being 
// included by the Windows.h header
#define WIN32_LEAN_AND_MEAN

// The Winsock2.h header file contains most of the Winsock functions, 
// structures, and definitions. The Ws2tcpip.h header file contains 
// definitions introduced in the WinSock 2 Protocol-Specific
// The Winsock2.h header file internally includes core elements from 
// the Windows.h header file
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

extern void InitString(char* buffer, int length);


DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern DWORD WINAPI thread_ServerInstance(SOCKET *lpParameter);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
	// Create a WSADATA object called wsaData.
	WSADATA wsaData;
	int iResult;
	SOCKET Server_Socket = INVALID_SOCKET;
	SOCKET Server_Soc[10];
	int count_socket=0;
	// Declare an addrinfo object that contains a sockaddr structure
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int recvbuflen = DEFAULT_BUFLEN;

	struct sockaddr_storage their_addr;
	size_t addr_len;
	char s[INET6_ADDRSTRLEN];

	message msg1;

	printf("Sistemas Distribuidos: Servidor MultiThread Inicializando ...\n");
	printf("Este Servidor aceita conexoes de multiplos Clientes\n");
	printf("Modo de Operacao: Servidor recebe uma mensagem do Cliente e retransmite de volta para o mesmo Cliente\n\n");

	SocketParams *params;
	params = (SocketParams *)lpParameter;

	//Initialize Winsock
	// The WSAStartup function is called to initiate use of WS2_32.dll
	// The MAKEWORD(2,2) parameter of WSAStartup makes a request for 
	// version 2.2 of Winsock on the system
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	// Internet address family is unspecified so that either an 
	// IPv6 or IPv4 address can be returned. The application requests 
	// the socket type to be a stream socket for the TCP protocol
	hints.ai_family = params->family;
	hints.ai_socktype = params->socktype;
	hints.ai_protocol = params->protocol;
	hints.ai_flags = params->flags;

	/* Resolve the server address and port */
	iResult = getaddrinfo(NULL, params->port, &hints, &result);

	if ( iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	/* Create a SOCKET for connecting to server
	After initialization, a SOCKET object must be instantiated for use by the server */
	Server_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Server_Socket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/* Setup the TCP listening socket
	For a server to accept client connections, it must be bound to a network 
	address within the system. The following code demonstrates how to bind a 
	socket that has already been created to an IP address and port. Client 
	applications use the IP address and port to connect to the host network. */
	iResult = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(Server_Socket);
		WSACleanup();
		return 1;
	}

	/* Listen
	After the socket is bound to an IP address and port on the system, 
	the server must then listen on that IP address and port for incoming 
	connection requests. */
	/*iResult = listen(Server_Socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(Server_Socket);
		WSACleanup();
		return 1;
	}*/

	/* Accept a client socket and create a thread to manage each accepted connection */
	while(true)
	{
		addr_len = sizeof(their_addr);
		iResult = recvfrom(Server_Socket, (char *)&msg1, (int)sizeof(msg1), 0, (struct sockaddr *)&their_addr, (int *) &addr_len);
	
		FILE *fp;
		char result[DEFAULT_BUFLEN];
		char* fileName = msg1.name;
		char* message = msg1.buf;
		char buffer[DEFAULT_BUFLEN];

		InitString(buffer, DEFAULT_BUFLEN);
		InitString(result, DEFAULT_BUFLEN);

		if (msg1.nro_msg == 1)
		{
			fp = fopen(fileName, "w");
			if (fp == NULL)
			{
				strcpy(result, "File could not be created.");
			}
			else
			{
				strcpy(result, "File successfully created.");
				fclose(fp);
			}
		}
		if (msg1.nro_msg == 2)
		{
			int status = remove(fileName);
			if (!result)
			{
				strcpy(result, "File could not be deleted.");
			}
			else
			{
				strcpy(result, "File successfully deleted.");
			}
		}
		if (msg1.nro_msg == 3)
		{
			fp = fopen(fileName, "r+");
			fscanf(fp, "%s", buffer);
			strcpy(result, buffer);
			fclose(fp);
		}
		if (msg1.nro_msg == 4)
		{
			fp = fopen(fileName, "a+");
			fprintf(fp, message);
			strcpy(result, "File succesfully written.");
			fclose(fp);
		}

		iResult = sendto(Server_Socket, (const char *)&result, (int)sizeof(result), 0, (struct sockaddr *)&their_addr, (int ) &addr_len);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(Server_Socket);
			WSACleanup();
			return 1;
		}
		//result = send(Server_Socket, (const char *)&result, DEFAULT_BUFLEN, 0);

		//ServerInstance = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) thread_ServerInstance, &Server_Soc[count_socket], 0, 0);
		//count_socket++;

		Sleep(10);
	}
	return 0;
}