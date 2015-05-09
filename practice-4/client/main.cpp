#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

extern DWORD WINAPI t_client(LPVOID lpParameter);

int main(int argc, char *argv[])
{
    SocketParams params;
    strcpy(params.port, CLIENT_PORT);

    params.family = AF_UNSPEC;
    params.socktype = SOCK_STREAM;
    params.protocol = IPPROTO_TCP;

    Cliente = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)t_client, &params, 0, 0);
    WaitForSingleObject(Cliente, INFINITE);

    return 0;
}
