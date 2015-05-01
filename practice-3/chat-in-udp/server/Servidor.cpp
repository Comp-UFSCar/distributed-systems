#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"

#pragma comment (lib, "Ws2_32.lib")

ClientList clientList;

void
cleanUp()
{
	for (int i = 0; i < 10; i++)
	{
		Client *client = &clientList.clients[i];
		if (client->socket == 0 && client->socket != INVALID_SOCKET)
		{
			closesocket(client->socket);
			client->socket = INVALID_SOCKET;
			client->used   = false;
		}
	}
}

void
initializeClientList(ClientList list)
{
	list.current = NULL;
	
	for (int i = 0; i < 10; i++)
	{
		Client *client = &list.clients[i];
		client->enabled = true;
		client->id = 0;
		client->socket = INVALID_SOCKET;
		client->used = false;
	}
}


DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern DWORD WINAPI thread_ServerInstance(ClientList *lpParameter);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
	WSADATA wsaData;
	int iResult;
	SOCKET Server_Socket = INVALID_SOCKET;
	Client clients[10];
	
	clientList.clients = clients;
	int count_socket = 0;

	initializeClientList(clientList);

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int recvbuflen = DEFAULT_BUFLEN;

	printf("Sistemas Distribuidos: Servidor MultiThread Inicializando ...\n");

	SocketParams *params;
	params = (SocketParams *)lpParameter;

	//Initialize Winsock
	// The WSAStartup function is called to initiate use of WS2_32.dll
	// The MAKEWORD(2,2) parameter of WSAStartup makes a request for 
	// version 2.2 of Winsock on the system
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	
    hints.ai_family = params->family;
	hints.ai_socktype = params->socktype;
	hints.ai_protocol = params->protocol;
	hints.ai_flags = params->flags;

	iResult = getaddrinfo(NULL, params->port, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
    Server_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Server_Socket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(Server_Socket);
		WSACleanup();
		return 1;
	}

	iResult = listen(Server_Socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(Server_Socket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		int i;
		for (i = 0; i < 10; i++) {
			if (!clientList.clients[i].used) {
				break;
			}
		}

		clientList.clients[i].id = i + 1;
		clientList.clients[i].used = clientList.clients[i].enabled = true;
		clientList.clients[i].socket = accept(Server_Socket, NULL, NULL);
		clientList.current = &clientList.clients[i];

		if (clientList.clients[i].socket == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(clientList.clients[i].socket);
			clientList.clients[i].used = false;
			WSACleanup();
			return 1;
		}
		else
		{
			ServerInstance = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_ServerInstance, &clientList, 0, 0);
			count_socket++;
		}
		Sleep(10);
	}

	return 0;
}
