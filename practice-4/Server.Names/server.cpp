#define WIN32_LEAN_AND_MEAN
#define TABLE_SIZE 30

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"
#include "..\Infrastructure\validation.h"
#include "..\Infrastructure\names_server_operations.h"

#pragma comment (lib, "Ws2_32.lib")

int index = 0;
NameEntry table[TABLE_SIZE];

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void tableRegister(char* name, char* ip, char* port)
{
    if (index == TABLE_SIZE)
    {
        printf("Cannot create entry for %s, since the table is empty.", name);
        return;
    }

    printf("Created entry {%s -> %s:%s}\n", name, ip, port);
 
    strcpy(table[index].name, name);
    strcpy(table[index].ip, ip);
    strcpy(table[index].port, port);

    index++;
}

NameEntry tableQuery(char* name)
{
    printf("Request for {%s}... ", name);

    for (int i = 0; i < index; i++)
    {
        if (!strcmp(name, table[i].name))
        {
            printf("found.\n");
            return table[i];
        }
    }

    printf("not found.\n");

    /// Returns an error message if server hasn't found the searched name.
    NameEntry error;
    strcpy(error.name, "Server not found!");
    strcpy(error.ip, "ERROR");
    strcpy(error.port, "ERROR");

    return error;
}

bool CheckOperation(NamesMessage *m)
{
    bool isOperation = false;
    if (m->operation == OPERATION_REGISTER)
    {
        isOperation = true;
        tableRegister(m->entry.name, m->entry.ip, m->entry.port);
    }
    else if (m->operation == OPERATION_QUERY)
    {
        isOperation = true;
        NameEntry entry = tableQuery(m->entry.name);
    }

    return isOperation;
}

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
    int response;
    SOCKET Server_Socket = INVALID_SOCKET;
    int count_socket = 0;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int recvbuflen = DEFAULT_BUFLEN;

    char s[INET6_ADDRSTRLEN];

    int flag_on = 1;

    printf("Names-server has started.\n");

    SocketParams *params;
    params = (SocketParams *)lpParameter;

    WSADATA wsaData;
    response = WSAStartup(MAKEWORD(2, 2), &wsaData);
    AssertZero(response, "WSAStartup");
    
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = params->family;
    hints.ai_socktype = params->socktype;
    hints.ai_protocol = params->protocol;
    hints.ai_flags = params->flags;

    response = getaddrinfo(NULL, params->port, &hints, &result);
    AssertZero(response, "getaddrinfo");

    Server_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    AssertValidSocket(Server_Socket, "Server_Socket");
    
    response = setsockopt(Server_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on, sizeof(flag_on));
    AssertPositive(response, "setsockopt");

    response = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
    AssertNotEquals(response, SOCKET_ERROR, "bind");
    
    while (running)
    {
        NamesMessage m;

        struct sockaddr_storage their_addr;
        size_t addr_len = sizeof their_addr;

        response = recvfrom(Server_Socket, (char *)&m, (int)sizeof(m), 0, (struct sockaddr *)&their_addr, (int *)&addr_len);
        if (response < 0) break;
        
        if (m.operation == OPERATION_REGISTER)
        {
            tableRegister(m.entry.name, m.entry.ip, m.entry.port);
        }
        else if (m.operation == OPERATION_QUERY)
        {
            m.entry = tableQuery(m.entry.name);
            response = sendto(Server_Socket, (const char *)&m, (int)sizeof(m), 0, (const struct sockaddr *)&their_addr, addr_len);

        }

        Sleep(10);
    }

    closesocket(Server_Socket);
    return 0;
}
