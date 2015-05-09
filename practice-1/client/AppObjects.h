

#ifndef APPOBJECTS_H
#define APPOBJECTS_H

#define _WIN32_WINNT 0x501
#include <windows.h>

extern HANDLE Cliente;
extern SOCKET ClientSocket;

enum OperationType {
	Create, Delete, Read, Write
};

#endif 

