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
    for (int i = 0; i < clientList.Length; i++)
    {
        Client *client = &clientList.clients[i];
        if (client->socketAddress != NULL)
        {
            free(client->socketAddress);
        }
    }

    if (clientList.socket != INVALID_SOCKET)
        closesocket(clientList.socket);
}

void
initializeClientList(ClientList list)
{
    list.socket = INVALID_SOCKET;
	
	for (int i = 0; i < 10; i++)
	{
		Client *client = &list.clients[i];
		client->enabled = true;
		client->id = 0;
        client->socketAddress = NULL;
		client->used = false;
	}
}


DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern void RunChat(ClientList *clientList);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
	WSADATA wsaData;
	int iResult;
    
    Client clients[10];
    clientList.Length = 10;
	clientList.clients = clients;

	initializeClientList(clientList);

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	printf("Sistemas Distribuidos: Servidor MultiThread Inicializando ...\n");

	SocketParams *params;
	params = (SocketParams *)lpParameter;

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
	if (iResult != 0)
    {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
    clientList.socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientList.socket == INVALID_SOCKET)
    {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

    RunChat(&clientList);
    
    return 0;
}
