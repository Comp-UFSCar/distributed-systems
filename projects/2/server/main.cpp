#include <signal.h>
#include <winsock2.h>
#include "AppObjects.h"
#include "my_socket.h"

#define DEFAULT_PORT "5000"

extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);
int main(void);

extern void cleanUp();

void intHandler(int dummy)
{
	cleanUp();
}

int main(void)
{
	signal(SIGINT, intHandler);
	SocketParams params;

	strcpy(params.port, DEFAULT_PORT);

	params.family   = AF_INET;
	params.socktype = SOCK_STREAM;
	params.protocol = IPPROTO_TCP;
	params.flags    = AI_PASSIVE;
	
	Servidor = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Servidor, &params, 0, 0);
	
	WaitForSingleObject(Servidor, INFINITE);

	exit(0);
}
