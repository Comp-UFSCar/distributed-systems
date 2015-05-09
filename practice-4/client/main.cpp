#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

extern DWORD WINAPI t_client(LPVOID lpParameter);
int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    SocketParams params;
    strcpy(params.port, CLIENT_PORT);

    if (argc != 2)
    {
        printf("modo de usar: cliente ip_do_servidor\n");
        exit(1);
    }

    params.ip = argv[1];

    params.family = AF_UNSPEC;
    params.socktype = SOCK_STREAM;
    params.protocol = IPPROTO_TCP;

    Cliente = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)t_client, &params, 0, 0);
    WaitForSingleObject(Cliente, INFINITE);

    getchar();
}
