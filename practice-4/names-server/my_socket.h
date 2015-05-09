#define DEFAULT_PORT "27015"
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
} entry;

typedef struct {
    entry entry;
    int operation;
} Message;