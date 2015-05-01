/* ServerInstance.cpp - Generated by Visual Multi-Thread Win32 */

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include "AppObjects.h"

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
	int messageLength;
	struct addrinfo *result = NULL;
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	SOCKET Server_Socket;
	Server_Socket = *lpParameter;

	message msg1;

	/* Receive until the peer shuts down the connection */
	while (true)
	{
		messageLength = recv(Server_Socket, (char *)&msg1, (int)sizeof(msg1), 0);

		// Check for error or the closing of the connection
		if (messageLength == 0) break;
		if (msg1.buf[0] == '0') { printf("Close: %s\n\n", msg1.name); break; }
		if (messageLength == -1) { printf("Close: %s\n\n", msg1.name); break; }

		/*printf("Server received-nome:  %s\n",msg1.name);
		printf("Server received-nro_msg:  %d\n",msg1.nro_msg);
		printf("Bytes  received: %d\n", messageLength);
		printf("Server received-message: %s\n",msg1.buf);*/

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