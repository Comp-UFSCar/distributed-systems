#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"
#include <string>
#include <cstdio>
#include <iostream>

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

void InitString(char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		buffer[i] = '\0';
	}
}

DWORD WINAPI thread_Cliente(LPVOID lpParameter);

DWORD WINAPI thread_Cliente(LPVOID lpParameter)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	message msg1;
	char scanTrash;
	char response[DEFAULT_BUFLEN];

	SocketParams *params;
	params = (SocketParams *)lpParameter;

	printf("Entre com o nome do Cliente: \n");
	gets(msg1.name);

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

	iResult = getaddrinfo(params->ip, params->port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		iResult = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ClientSocket);
			ClientSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	char status = 'A';
	while (status != 'f')
	{
		printf("Insert file name: ");
		gets(msg1.name);

		printf("Insert operation (1-Create, 2-Delete, 3-Read, 4-Write): ");
		scanf("%i", &msg1.nro_msg);
		scanf("%c", &scanTrash);
		if (msg1.nro_msg == 4)
		{
			printf("Insert your message: ");
			gets(msg1.buf);
		}
		else
		{
			msg1.buf[0] = '\0';
		}

		iResult = send(ClientSocket, (const char *)&msg1, (int)sizeof(msg1), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		printf("Client - Bytes Sent: %ld\n", iResult);

		InitString(response, DEFAULT_BUFLEN);
		iResult = recv(ClientSocket, (char *)&response, DEFAULT_BUFLEN, 0);

		printf("Client received-message: %s\n\n", response);

		Sleep(10);

		printf("Tecle: f - Para terminar ou c - Para enviar outra mensagem \n");
		status = 'A';
		while ((status != 'f') && (status != 'c'))
		{
			status = getchar();  Sleep(10);
			getchar();
		}
	}

	msg1.buf[0] = '0';
	iResult = send(ClientSocket, (const char *)&msg1, (int)sizeof(msg1), 0);
	closesocket(ClientSocket);
	return 0;
}
