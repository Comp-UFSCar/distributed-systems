#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "..\Infrastructure\communication.h"

#pragma comment (lib, "Ws2_32.lib")

void InitString(char* buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        buffer[i] = '\0';
    }
}

DWORD WINAPI t_server_instance(SOCKET *lpParameter)
{
    Message m;
    SOCKET t_socket = (SOCKET)*lpParameter;

    while (true)
    {
        int bytes = recv(t_socket, (char *)&m, (int)sizeof(m), 0);

        if (bytes == 0) break;
        if (m.buf[0] == '0')
        {
            printf("Close: %s\n\n", m.name);
            break;
        }
        if (bytes == -1)
        {
            printf("Close: %s\n\n", m.name);
            break;
        }

        FILE *fp;
        char answer[DEFAULT_BUFLEN];
        char buffer[DEFAULT_BUFLEN];

        InitString(buffer, DEFAULT_BUFLEN);
        InitString(answer, DEFAULT_BUFLEN);

        if (m.nro_msg == 1)
        {
            fp = fopen(m.name, "w");
            if (fp == NULL)
            {
                strcpy(answer, "File could not be created.");
            }
            else
            {
                strcpy(answer, "File successfully created.");
                fclose(fp);
            }
        }
        if (m.nro_msg == 2)
        {
            int status = remove(m.name);
            if (!answer)
            {
                strcpy(answer, "File could not be deleted.");
            }
            else
            {
                strcpy(answer, "File successfully deleted.");
            }
        }
        if (m.nro_msg == 3)
        {
            fp = fopen(m.name, "r+");
            fscanf(fp, "%s", buffer);
            strcpy(answer, buffer);
            fclose(fp);
        }
        if (m.nro_msg == 4)
        {
            fp = fopen(m.name, "a+");
            fprintf(fp, m.buf);
            strcpy(answer, "File succesfully written.");
            fclose(fp);
        }

        bytes = send(t_socket, (const char *)&answer, DEFAULT_BUFLEN, 0);
        if (bytes == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(t_socket);
            WSACleanup();
            return 1;
        }

        Sleep(10);
    }

    closesocket(t_socket);
    return 0;
}
