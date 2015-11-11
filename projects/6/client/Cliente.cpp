#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"
#include <string>
#include <cstdio>

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

DWORD WINAPI thread_Cliente(LPVOID lpParameter)
{
    WSADATA wsaData;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    sockaddr from;
    int from_length;
    int iResult;
    Message message;

    SocketParams *params;
    params = (SocketParams *)lpParameter;

    printf("Your name: ");
    gets(message.name);

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    SOCKET sync_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (sync_s == INVALID_SOCKET) {
        exit(1);
    }

    struct sockaddr_in sync_server_address;
    ZeroMemory(&sync_server_address, sizeof(sync_server_address));
    sync_server_address.sin_family = AF_INET;
    sync_server_address.sin_addr.s_addr = inet_addr(SYNC_SERVER_ADDRESS);
    sync_server_address.sin_port = htons(SYNC_SERVER_PORT);

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

    char request_lock[64], request_unlock[64], answer[64];
    strcpy(request_lock, "lock write-server");
    strcpy(request_unlock, "unlock write-server");

    while (1)
    {
        printf("Insert your message: ");
        gets(message.buffer);

        sendto(sync_s, request_lock, strlen(request_lock), 0, (struct sockaddr *)&sync_server_address, sizeof(sync_server_address));
        int n = recvfrom(sync_s, answer, 64, 0, NULL, NULL);
        answer[n] = 0;
        
        printf("Permission granted for resource: %s.\n", answer);

        int length = strlen(message.buffer);

        for (int i = 0; i < length; i++)
        {
            Sleep(100);

            int response = send(ClientSocket, (const char *)&message.buffer[i], (int)sizeof(char), 0);
            if (response == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }

        int response = send(ClientSocket, (const char *)&"\n", (int)sizeof(char), 0);
        if (response == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        sendto(sync_s, request_unlock, strlen(request_unlock), 0, (struct sockaddr *)&sync_server_address, sizeof(sync_server_address));
    }

    closesocket(ClientSocket);
    closesocket(sync_s);

    return 0;
}
