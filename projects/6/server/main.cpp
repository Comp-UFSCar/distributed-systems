#include <winsock2.h>
#include "AppObjects.h"
#include "my_socket.h"
#define DEFAULT_PORT "5000"

extern DWORD WINAPI thread_Servidor(LPVOID lpParameter);
void main(void);

void main(void)
{
    SocketParams params;

    strcpy(params.port, DEFAULT_PORT);

    params.family   = AF_INET;
    params.socktype = SOCK_STREAM;
    params.protocol = IPPROTO_TCP;
    params.flags    = AI_PASSIVE;
    HANDLE server_thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_Servidor, &params, 0, 0);
    WaitForSingleObject(server_thread, INFINITE);
}
