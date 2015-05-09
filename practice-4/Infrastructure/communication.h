#define NAMES_SERVER_PORT "7836"
#define FILES_SERVER_PORT "27015"
#define CLIENT_PORT "9872"

#define DEFAULT_BUFLEN 512

#define OPERATION_REGISTER 10
#define OPERATION_RETRIEVE 11

typedef struct
{
    char* ip;
    char port[6];
    int family;
    int socktype;
    int protocol;
    int flags;
} SocketParams;

typedef struct {
    char name[30];
    char ip[30];
    char port[6];
} NameEntry;

typedef struct {
    NameEntry entry;
    int operation;
} NamesMessage;

typedef struct
{
    char name[15];
    int nro_msg;
    char buf[DEFAULT_BUFLEN];
} Message;
