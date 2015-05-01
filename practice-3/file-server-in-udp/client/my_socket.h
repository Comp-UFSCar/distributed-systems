#define DEFAULT_BUFLEN 512


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
	int  nro_msg;
	char buf[DEFAULT_BUFLEN];
} message;
