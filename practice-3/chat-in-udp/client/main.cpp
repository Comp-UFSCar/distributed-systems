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

void intHandler(int dummy)
{
    cleanUp();
    printf("\nBye\n");
}

void openOurVerySocket(SocketParams *params) {
    WSADATA wsaData;
    struct addrinfo *result = NULL, hints;

    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }

    ZeroMemory(&hints, sizeof(hints));
    
    hints.ai_family = params->family;
    hints.ai_socktype = params->socktype;
    hints.ai_protocol = params->protocol;

    iResult = getaddrinfo(params->ip, params->port, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        exit(1);
    }

    Channel.addressInfo = result;
    Channel.socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (Channel.socket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
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
    params.socktype = SOCK_DGRAM;
    params.protocol = IPPROTO_UDP;
    
    openOurVerySocket(&params);

    ClienteSend = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Send, 0, 0, 0);
    ClienteReceive = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Receive, 0, 0, 0);

    WaitForSingleObject(ClienteSend, INFINITE);
    WaitForSingleObject(ClienteReceive, INFINITE);

    cleanUp();

    getchar();
    return 0;
}
