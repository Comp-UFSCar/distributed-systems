#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "my_socket.h"
#define DEFAULT_PORT "5000"

extern DWORD WINAPI thread_Cliente(LPVOID lpParameter);

int main(int argc, char *argv[])
{
    SocketParams params;
    
    strcpy(params.port, DEFAULT_PORT);

    if (argc == 1)
        params.ip = "127.0.0.1";
    else if (argc == 2)
	    params.ip = argv[1];

	params.family = AF_UNSPEC;	    
	params.socktype = SOCK_STREAM; 
	params.protocol = IPPROTO_TCP;	 

	Cliente = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Cliente, &params, 0, 0);
	WaitForSingleObject(Cliente, INFINITE);

	getchar();
}
