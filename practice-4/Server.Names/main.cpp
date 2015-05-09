#include <winsock2.h>
#include "AppObjects.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "..\Infrastructure\communication.h"
#include "..\Infrastructure\names_server_operations.h"


extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);

void intHandler(int dummy)
{
    running = false;

    if (Server_Socket)
    {
        closesocket(Server_Socket);
        Server_Socket = INVALID_SOCKET;
    }

    WSACleanup();
}

int main(int argc, char *argv[])
{
    if (signal(SIGINT, intHandler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

	SocketParams params;
	
	strcpy(params.port, NAMES_SERVER_PORT);
	
	params.family = AF_INET;
	params.socktype = SOCK_DGRAM;
	params.protocol= IPPROTO_UDP;
	params.flags = AI_PASSIVE;
	
	Servidor = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) thread_Servidor, &params, 0, 0);
	WaitForSingleObject(Servidor, INFINITE);
}
