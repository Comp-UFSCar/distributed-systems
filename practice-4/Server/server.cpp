#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"
#include "..\Infrastructure\validation.h"
#include "..\Infrastructure\names_server_operations.cpp"

#pragma comment (lib, "Ws2_32.lib")

void RegisterName(NameEntry *entry)
{
    int response;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    printf("Registering name on names-server... ");

    char ip[16] = "255.255.255.255";

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    response = getaddrinfo("255.255.255.255", NAMES_SERVER_PORT, &hints, &ptr);
    AssertZero(response, "getaddrinfo");

    SOCKET namesServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    AssertValidSocket(namesServerSocket, "name-server socket");

    /// Set time-to-live 1.
    unsigned char mc_ttl = 1;
    response = setsockopt(namesServerSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&mc_ttl, sizeof(mc_ttl));
    AssertPositive(response, "setsockopt");

    NamesMessage m;
    m.entry = *entry;
    m.operation = OPERATION_REGISTER;

    response = sendto(namesServerSocket, (const char *)&m, (int)sizeof(m), 0, ptr->ai_addr, ptr->ai_addrlen);
    AssertNotEquals(response, SOCKET_ERROR, "sendto");

    closesocket(namesServerSocket);

    printf("Done.\n");
}

DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern DWORD WINAPI thread_ServerInstance(SOCKET *lpParameter);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
    int response;
    SOCKET Server_Socket = INVALID_SOCKET;
    SOCKET Server_Soc[10];
    int count_socket = 0;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int recvbuflen = DEFAULT_BUFLEN;

    printf("Initiating file-server... \n");

    SocketParams *params;
    params = (SocketParams *)lpParameter;

    WSADATA wsaData;
    response = WSAStartup(MAKEWORD(2, 2), &wsaData);
    AssertZero(response, "WSAStartup");

    NameEntry entry;
    strcpy(entry.name, FILES_SERVER_NAME);
    strcpy(entry.ip, "127.0.0.1");
    strcpy(entry.port, params->port);

    RegisterName(&entry);

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = params->family;
    hints.ai_socktype = params->socktype;
    hints.ai_protocol = params->protocol;
    hints.ai_flags = params->flags;

    response = getaddrinfo(NULL, params->port, &hints, &result);
    AssertZero(response, "getaddrinfo");
    
    Server_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    AssertValidSocket(Server_Socket, "socket");
    
    response = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
    AssertNotEquals(response, SOCKET_ERROR, "bind");
    
    response = listen(Server_Socket, SOMAXCONN);
    AssertNotEquals(response, SOCKET_ERROR, "listen");

    while (true)
    {
        printf("Waiting...\n");

        Server_Soc[count_socket] = accept(Server_Socket, NULL, NULL);
        if (Server_Soc[count_socket] == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", INVALID_SOCKET);
            closesocket(Server_Soc[count_socket]);
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
