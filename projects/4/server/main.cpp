#include <winsock2.h>
#include <stdio.h>
#include <signal.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

extern DWORD WINAPI t_server(LPVOID lpParameter);


void handler(int dummy)
{
    if (ServerSocket != INVALID_SOCKET)
    {
        closesocket(ServerSocket);
        WSACleanup();

        ServerSocket = INVALID_SOCKET;

        printf("\nbye.\n");
    }
}

int main(void)
{
    signal(SIGINT, handler);
    
    SocketParams params;
	
	strcpy(params.port, FILES_SERVER_PORT);
	
	params.family = AF_INET;
	params.socktype = SOCK_STREAM;
	params.protocol= IPPROTO_TCP;
	params.flags = AI_PASSIVE;

	Servidor = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) t_server, &params, 0, 0);
	WaitForSingleObject(Servidor, INFINITE);

    handler(0);
    return 0;
}
