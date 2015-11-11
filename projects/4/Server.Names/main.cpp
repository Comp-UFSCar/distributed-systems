#include <winsock2.h>
#include "AppObjects.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "..\Infrastructure\communication.h"
#include "..\Infrastructure\names_server_operations.h"


extern DWORD WINAPI t_server(LPVOID lpParameter);

void handler(int dummy)
{
    if (ServerSocket != INVALID_SOCKET)
    {
        closesocket(ServerSocket);
        ServerSocket = INVALID_SOCKET;
    
        printf("\nbye.\n");
    }

    WSACleanup();
}

int main(int argc, char *argv[])
{
    if (signal(SIGINT, handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

	SocketParams params;
	
	strcpy(params.port, NAMES_SERVER_PORT);
	
	params.family = AF_INET;
	params.socktype = SOCK_DGRAM;
	params.protocol= IPPROTO_UDP;
	params.flags = AI_PASSIVE;
	
	Servidor = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) t_server, &params, 0, 0);
	WaitForSingleObject(Servidor, INFINITE);

    handler(0);
    return 0;
}
