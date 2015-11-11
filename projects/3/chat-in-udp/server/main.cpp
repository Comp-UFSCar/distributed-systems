#include <signal.h>
#include <winsock2.h>
#include "AppObjects.h"
#include "my_socket.h"

#define DEFAULT_PORT "5000"

extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);
extern void         cleanUp(); 
int                 main(void);

void
intHandler(int dummy)
{
	cleanUp();
}

int
main(void)
{
	signal(SIGINT, intHandler);
	
    SocketParams params;

	strcpy(params.port, DEFAULT_PORT);

	params.family   = AF_INET;
	params.socktype = SOCK_DGRAM;
	params.protocol = IPPROTO_UDP;
	params.flags    = AI_PASSIVE;
	
	Servidor = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Servidor, &params, 0, 0);
	
	WaitForSingleObject(Servidor, INFINITE);

    return 0;
}
