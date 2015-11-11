#ifndef VALIDATION
#define VALIDATION

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

bool AssertTrue(bool condition, char *module)
{
    if (!condition)
    {
        if (module && module[0] != '\0')
            printf("error: %s ", module);
        else
            printf("error: ");
    }

    return condition;
}

void Fail(char *module)
{
    AssertTrue(false, module);
}

void AssertZero(int response, char *module)
{
    if (response)
    {
        Fail(module);
        printf("raised {%d}.\n", response);
        exit(1);
    }
}

void AssertValidSocket(SOCKET s, char *module)
{
    if (s == INVALID_SOCKET)
    {
        Fail(module);
        printf("is an INVALID_SOCKET.\n");
        exit(1);
    }
}

void AssertNotEquals(int a, int b, char *module)
{
    bool result = AssertTrue(a != b, module);
    if (!result) exit(1);
}

void AssertPositive(int a, char *module)
{
    bool result = AssertTrue(a >= 0, module);
    if (!result) exit(1);
}

#endif
