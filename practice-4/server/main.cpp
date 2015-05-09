#include <winsock2.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);
void main(void);

void main(void)
{
	SocketParams params;
	
	strcpy(params.port, FILES_SERVER_PORT);
	
	params.family = AF_INET;
	params.socktype = SOCK_STREAM;
	params.protocol= IPPROTO_TCP;
	params.flags = AI_PASSIVE;

	Servidor = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) thread_Servidor, &params, 0, 0);
	WaitForSingleObject(Servidor, INFINITE);
}
