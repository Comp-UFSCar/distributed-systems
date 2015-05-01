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

DWORD WINAPI thread_Receive(LPVOID lpParameter);
DWORD WINAPI thread_Send(LPVOID lpParameter);

void cleanUp()
{
    keepRunning = 0;

    if (Channel.socket != NULL)
        closesocket(Channel.socket);
}

bool checkOperation(message message)
{
    return !strcmp(message.buf, "*disable") || !strcmp(message.buf, "*enable");
}

DWORD WINAPI thread_Receive(LPVOID lpParameter)
{
    message message;
    sockaddr theirs;
    int size;

    while (keepRunning)
    {
        int result = recvfrom(
            Channel.socket, (char *)&message, sizeof(message), 0,
            &theirs, &size
        );

        if (keepRunning && result > 0)
            printf("%s:  %s\n>> ", message.name, message.buf);

        Sleep(10);
    }

    return 0;
}

DWORD WINAPI thread_Send(LPVOID lpParameter)
{
    message message;
    int result;
    char trash;

    printf("Entre com o nome do Cliente: \n");
    gets(message.name);

    message.nro_msg = 1;

    while (keepRunning)
    {
        printf(">> ");
        gets(message.buf);

        result = sendto(
            Channel.socket,
            (const char *)&message, sizeof(message), 0,
            Channel.addressInfo->ai_addr,
            Channel.addressInfo->ai_addrlen
        );
        if (result == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        Sleep(10);
    }

    message.buf[0] = '0';
    result = sendto(
        Channel.socket,
        (const char *)&message, (int)sizeof(message), 0,
        Channel.addressInfo->ai_addr,
        Channel.addressInfo->ai_addrlen
    );
    return 0;
}
