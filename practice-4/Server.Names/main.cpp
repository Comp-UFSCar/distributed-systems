#include <winsock2.h>
#include "AppObjects.h"
#include <stdio.h>
#include <stdlib.h>
#include "..\Infrastructure\communication.h"


extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);
int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	SocketParams params;
	
	strcpy(params.port, NAMES_SERVER_PORT);
	
	params.family = AF_INET;
	params.socktype = SOCK_DGRAM;
	params.protocol= IPPROTO_UDP;
	params.flags = AI_PASSIVE;
	
	Servidor = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) thread_Servidor, &params, 0, 0);
	WaitForSingleObject(Servidor, INFINITE);
}
