#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

void InitString(char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		buffer[i] = '\0';
	}
}

DWORD WINAPI thread_ServerInstance(SOCKET *lpParameter);

DWORD WINAPI thread_ServerInstance(SOCKET *lpParameter)
{
	int MessageLength;
	struct addrinfo *result = NULL;
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	SOCKET Server_Socket;
	Server_Socket = *lpParameter;

	Message msg1;

	while (true)
	{
		MessageLength = recv(Server_Socket, (char *)&msg1, (int)sizeof(msg1), 0);

		if (MessageLength == 0) break;
		if (msg1.buf[0] == '0') { printf("Close: %s\n\n", msg1.name); break; }
		if (MessageLength == -1) { printf("Close: %s\n\n", msg1.name); break; }

		FILE *fp;
		char result[DEFAULT_BUFLEN];
		char* fileName = msg1.name;
		char* Message = msg1.buf;
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
			fprintf(fp, Message);
			strcpy(result, "File succesfully written.");
			fclose(fp);
		}

		iSendResult = send(Server_Socket, (const char *)&result, DEFAULT_BUFLEN, 0);

		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(Server_Socket);
			WSACleanup();
			return 1;
		}
		printf("Bytes sent: %d\n", iSendResult);
		printf("Server sent = %s\n\n", result);

		Sleep(10);
	}
	closesocket(Server_Socket);
	return 0;
}
