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

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool checkOperation(message message, Client *client){
	if (!strcmp(message.buf, CLIENT_OP_MUTE)) {
		client->enabled = false;
		return true;
	}
	if (!strcmp(message.buf, CLIENT_OP_RESTORE)){
		client->enabled = true;
		return true;
	}

	return false;
}

Client*
FindOrCreate(ClientList *clientList, sockaddr socketAddress)
{
    Client *empty = NULL;
    
    for (int i = 0; i < 10; i++)
    {
        Client *current = &clientList->clients[i];

        if (!current->used)
        {
            // Find first position available.
            if (empty == NULL) empty = current;
        }
        else
        {
            sockaddr *currentSocketAddress = current->socketAddress;

            // Compare both addresses...
            // if equal, return current. Continue, otherwise.
        }
    }

    // Client not found (it's a new client). Allocate all shit for him.
    if (empty != NULL)
    {
        sockaddr *newSocketAddress = (sockaddr *)malloc(sizeof(sockaddr));
        strcpy(newSocketAddress->sa_data, socketAddress.sa_data);
        newSocketAddress->sa_family = socketAddress.sa_family;
    }

    return empty;
}

void
removeClient(Client *client)
{
	client->id = 0;
	client->used = false;
	client->enabled = true;
}

void
sendAll(ClientList *clientList, Client *current, message message)
{
	for (int i = 0; i < 10; i++)
	{
		Client *client = &clientList->clients[i];

		if (!client->used || !client->enabled || i == current->id - 1
			|| client->socketAddress == NULL) {
			continue;
		}

        /*int result = sendto(
            clientList->socket,
            (const char *)&message, sizeof(message), 0,            
            client->addressInfo->ai_addr, client->addressInfo->ai_addrlen
        );
		if (result == SOCKET_ERROR)
        {
			printf("Send failed with error: %d\n", WSAGetLastError());
			removeClient(client);
			WSACleanup();
		}*/
	}
}

void
RunChat(ClientList *clientList)
{
    int size;
    sockaddr clientSocketAddress;
    message message;

    while (true)
    {
        int result = recvfrom(clientList->socket, (char *)&message, sizeof(message), 0, &clientSocketAddress, &size);
        
        // This might be result <= 0. I'm not sure if result is a status-code or the length of the retrieved data.
        if (result < 0) continue;

        Client *current = FindOrCreate(clientList, clientSocketAddress);

        if (message.buf[0] == '0')
        {
            printf("Close: %s\n\n", message.name);
        }
        if (result == -1) {
            printf("Close: %s\n\n", message.name);
            removeClient(current);
            continue;
        }

        printf("%s:  %s\n", message.name, message.buf);

        bool isOperation = checkOperation(message, current);
        if (!isOperation) {
            sendAll(clientList, current, message);
        }

        Sleep(10);
    }
}