#define WIN32_LEAN_AND_MEAN
#define TABLE_SIZE 30

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"

#pragma comment (lib, "Ws2_32.lib")

int index = 0;
entry table[TABLE_SIZE];

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void tableRegister(char* name, char* ip, char* port)
{
    if (index <= TABLE_SIZE)
    {
        strcpy(table[index].name, name);
        strcpy(table[index].ip, ip);
        strcpy(table[index].port, port);

        index++;
    }
}

entry tableQuery(char* name) {
    for (int i = 0; i < index; i++)
        if (strcmp(name, table[i].name))
            return table[i];

    // Returns an error Message if server not found by name
    entry error;
    strcpy(error.name, "Server not found!");
    strcpy(error.ip, "ERROR");
    strcpy(error.port, "ERROR");

    return error;
}

bool CheckOperation(Message *message)
{
    bool isOperation = false;
    if (message->operation == OPERATION_REGISTER)
    {
        isOperation = true;
        tableRegister(message->entry.name, message->entry.ip, message->entry.port);
    }
    else if (message->operation == OPERATION_RETRIEVE)
    {
        isOperation = true;
        tableQuery(message->entry.name);
    }

    return isOperation;
}

DWORD WINAPI thread_Servidor(LPVOID lpParameter);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
    WSADATA wsaData;
    int iResult;
    SOCKET Server_Socket = INVALID_SOCKET;
    int count_socket = 0;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int recvbuflen = DEFAULT_BUFLEN;

    struct sockaddr_storage their_addr;
    size_t addr_len;
    char s[INET6_ADDRSTRLEN];

    int flag_on = 1;

    printf("Sistemas Distribuidos: Servidor Inicializando ...\n\n");

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
    if (Server_Socket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if ((setsockopt(Server_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on,
        sizeof(flag_on))) < 0)
    {
        perror("setsockopt() failed");
        exit(1);
    }

    iResult = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(Server_Socket);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        Message message;

        printf("listener: waiting to recvfrom...\n");
        addr_len = sizeof their_addr;

        iResult = recvfrom(Server_Socket, (char *)&message, (int)sizeof(message), 0, (struct sockaddr *)&their_addr, (int *)&addr_len);
        if (iResult == -1)
        {
            printf("Erro-Close\n\n");
            break;
        }

        bool isOperation = CheckOperation(&message);

        printf("Listener: got packet from %s\n", inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
        printf("Listener: packet is %d bytes long\n", iResult);

        Sleep(10);
    }

    closesocket(Server_Socket);
    return 0;
}
