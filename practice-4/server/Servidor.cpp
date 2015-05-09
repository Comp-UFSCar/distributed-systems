#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

#pragma comment (lib, "Ws2_32.lib")


DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern DWORD WINAPI thread_ServerInstance(SOCKET *lpParameter);

void Validate(char *module, int number)
{
    if (number)
    {
        printf("error: %s raised {%d}.\n", module, number);
        exit(1);
    }
}

void RegisterName()
{
    int response;
    SocketParams params;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    printf("Registering name on names-server... ");

    strcpy(params.port, "27015");
    strcpy(params.ip, "255.255.255.255");
    params.family = AF_UNSPEC;
    params.socktype = SOCK_DGRAM;
    params.protocol = IPPROTO_UDP;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    response = getaddrinfo(params.ip, params.port, &hints, &ptr);
    Validate("getaddrinfo", response);

    SOCKET namesServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (namesServerSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        exit(1);
    }

    /// Set time-to-live 1.
    unsigned char mc_ttl = 1;
    if ((setsockopt(namesServerSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&mc_ttl, sizeof(mc_ttl))) < 0)
    {
        perror("error: setsockopt raised an unknown error.");
        exit(1);
    }

    Message m;

    int iResult = sendto(namesServerSocket, (const char *)&m, (int)sizeof(m), 0, ptr->ai_addr, ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(namesServerSocket);
        WSACleanup();
        exit(1);
    }

    printf("Done.");
}

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
    WSADATA wsaData;
    int iResult;
    SOCKET Server_Socket = INVALID_SOCKET;
    SOCKET Server_Soc[10];
    int count_socket = 0;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int recvbuflen = DEFAULT_BUFLEN;

    printf("Initiating file-server... \n");

    RegisterName();

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
        Server_Soc[count_socket] = accept(Server_Socket, NULL, NULL);
        if (Server_Soc[count_socket] == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(Server_Soc[count_socket]);
            WSACleanup();
            return 1;
        }
        else
        {
            ServerInstance = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_ServerInstance, &Server_Soc[count_socket], 0, 0);
            count_socket++;
        }

        Sleep(10);
    }
    return 0;
}
