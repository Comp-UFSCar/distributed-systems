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

///
/// Find a client by their address.
Client*
FindOrCreate(ClientList *clientList, sockaddr *socketAddress)
{
    Client *empty = NULL;
    
    for (int i = 0; i < clientList->Length; i++)
    {
        Client *current = &clientList->clients[i];

        // Find first position available.
        if (!current->used)
        {
            if (empty == NULL) empty = current;
        }
        else
        {
            if (strcmp(socketAddress->sa_data, current->socketAddress) == 0)
            {
                return current;
            }
        }
    }

    // Client not found (it's a new client). Allocate position for them.
    if (empty != NULL)
    {
        strcpy(empty->socketAddress, socketAddress->sa_data);
        empty->used = true;
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
	for (int i = 0; i < clientList->Length; i++)
	{
		Client *client = &clientList->clients[i];

		if (client == current
            || !client->used || !client->enabled
            || client->socketAddress == NULL)
            continue;

        // create socket address based on sa_data.
        sockaddr client_sock;       
        strcpy(client_sock.sa_data, client->socketAddress);
        client_sock.sa_family = AF_INET;
        int length = strlen(client_sock.sa_data);

        int result = sendto(
            clientList->socket, (const char *)&message, sizeof(message), 0,
            &client_sock, length
        );
		
        if (result == SOCKET_ERROR)
        {
			printf("Send failed with error: %d\n", WSAGetLastError());
			removeClient(client);
			WSACleanup();
		}
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
        if (result < 0) continue;

        Client *current = FindOrCreate(clientList, &clientSocketAddress);

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
        
        if (!isOperation)
            sendAll(clientList, current, message);

        Sleep(10);
    }
}
