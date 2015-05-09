#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

extern DWORD WINAPI t_client(LPVOID lpParameter);

void handler(int dummy)
{
    if (ClientSocket != INVALID_SOCKET)
    {
        closesocket(ClientSocket);
        WSACleanup();
        
        ClientSocket = INVALID_SOCKET;

        printf("\nbye.\n");
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handler);

    SocketParams params;
    strcpy(params.port, CLIENT_PORT);

    params.family = AF_UNSPEC;
    params.socktype = SOCK_STREAM;
    params.protocol = IPPROTO_TCP;

    Cliente = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)t_client, &params, 0, 0);
    WaitForSingleObject(Cliente, INFINITE);

    handler(0);
    return 0;
}
