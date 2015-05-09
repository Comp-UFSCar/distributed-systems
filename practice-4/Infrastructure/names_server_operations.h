#ifndef NAMES_SERVER_OPERATIONS
#define NAMES_SERVER_OPERATIONS

#define OPERATION_REGISTER 10
#define OPERATION_QUERY    11

#define NAMES_SERVER_PORT "7836"
#define FILES_SERVER_NAME "fileserver.com"
#define FILES_NAMES_PORT  "27216"
#define CLIENT_NAMES_PORT "9873"

typedef struct {
    char name[30];
    char ip[30];
    char port[6];
} NameEntry;

typedef struct {
    NameEntry entry;
    int operation;
} NamesMessage;

extern void RegisterName(NameEntry *entry);
extern NameEntry QueryName(char *name);

#endif