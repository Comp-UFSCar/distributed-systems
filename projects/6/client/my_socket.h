#define DEFAULT_BUFLEN 512
#define SYNC_SERVER_ADDRESS "127.0.0.1"
#define SYNC_SERVER_PORT 7989

typedef struct
{
	char *ip;
	char port[6];
	int family;
	int socktype;
	int protocol;
	int flags;
} SocketParams;

typedef struct
{
	char name[15];
	char buffer[DEFAULT_BUFLEN];
} Message;
