#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstdio>
#include <iostream>
#include "AppObjects.h"
#include "..\Infrastructure\validation.h"
#include "..\Infrastructure\communication.h"
#include "..\Infrastructure\names_server_operations.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

NameEntry QueryName(char *name)
{
    int serverResponse;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    printf("Querying %s on names-server... ", name);

    char ip[16] = "255.255.255.255";

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    serverResponse = getaddrinfo("255.255.255.255", NAMES_SERVER_PORT, &hints, &ptr);
    AssertZero(serverResponse, "getaddrinfo");

    SOCKET namesServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    AssertValidSocket(namesServerSocket, "name-server socket");

    /// Set time-to-live 1.
    unsigned char mc_ttl = 1;
    serverResponse = setsockopt(namesServerSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&mc_ttl, sizeof(mc_ttl));
    AssertPositive(serverResponse, "setsockopt");

    NamesMessage m;
    
    m.operation = OPERATION_QUERY;
    strcpy(m.entry.ip, "127.0.0.1");
    strcpy(m.entry.port, CLIENT_UDP_PORT);
    strcpy(m.entry.name, name);

    /// Asks which one is the file server.
    serverResponse = sendto(namesServerSocket, (const char *)&m, (int)sizeof(m), 0, ptr->ai_addr, ptr->ai_addrlen);
    AssertNotEquals(serverResponse, SOCKET_ERROR, "sendto");

    /// Retrieve its answer.
    struct sockaddr_storage their_addr;
    size_t addr_len = sizeof their_addr;

    serverResponse = recvfrom(namesServerSocket, (char *)&m, (int)sizeof(m), 0, (struct sockaddr *)&their_addr, (int *)&addr_len);
    AssertPositive(serverResponse, "recvfrom");

    closesocket(namesServerSocket);
    
    printf("Done.\n");
    return m.entry;
}

DWORD WINAPI t_client(LPVOID lpParameter)
{
    SocketParams *params = (SocketParams *)lpParameter;

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int recvbuflen = DEFAULT_BUFLEN;
    
    char scanTrash;
    char serverResponse[DEFAULT_BUFLEN];

    WSADATA wsaData;
    int response = WSAStartup(MAKEWORD(2, 2), &wsaData);
    AssertZero(response, "WSAStartup");
    
    Message m;

    printf("Enter your name: ");
    gets(m.name);

    char fileServerName[50];
    printf("Enter the name of the file-server that you wish to connect: ");
    gets(fileServerName);
    
    NameEntry entry = QueryName(fileServerName);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = params->family;
    hints.ai_socktype = params->socktype;
    hints.ai_protocol = params->protocol;

    response = getaddrinfo(entry.ip, entry.port, &hints, &result);
    AssertZero(response, "getaddrinfo");

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        AssertValidSocket(ClientSocket, "ClientSocket");
     
        response = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (response == SOCKET_ERROR)
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
        gets(m.name);

        printf("Insert operation (1-Create, 2-Delete, 3-Read, 4-Write): ");
        scanf("%i", &m.nro_msg);
        scanf("%c", &scanTrash);
        if (m.nro_msg == 4)
        {
            printf("Insert your Message: ");
            gets(m.buf);
        }
        else
        {
            m.buf[0] = '\0';
        }

        response = send(ClientSocket, (const char *)&m, (int)sizeof(m), 0);
        if (response == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", response);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        printf("Client - Bytes Sent: %ld\n", response);

        for (int i = 0; i < DEFAULT_BUFLEN; i++) serverResponse[i] = '\0';

        response = recv(ClientSocket, (char *)&serverResponse, DEFAULT_BUFLEN, 0);

        printf("Client received-Message: %s\n\n", serverResponse);

        Sleep(10);

        printf("Tecle: f - Para terminar ou c - Para enviar outra mensagem \n");
        status = 'A';
        while ((status != 'f') && (status != 'c'))
        {
            status = getchar();  Sleep(10);
            getchar();
        }
    }

    m.buf[0] = '0';
    response = send(ClientSocket, (const char *)&m, (int)sizeof(m), 0);
    closesocket(ClientSocket);
    return 0;
}
