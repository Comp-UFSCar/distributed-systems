#ifndef APPOBJECTS_H
#define APPOBJECTS_H

#define _WIN32_WINNT 0x501
#include <windows.h>

extern HANDLE ClienteSend;
extern HANDLE ClienteReceive;

static volatile int keepRunning = 1;

typedef struct {
    SOCKET socket;
    struct addrinfo *addressInfo;
} ClientConnectionChannel;

extern ClientConnectionChannel Channel;


#endif
