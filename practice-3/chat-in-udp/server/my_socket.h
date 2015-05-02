#define DEFAULT_BUFLEN 512

#define CLIENT_OP_MUTE "*mute"
#define CLIENT_OP_RESTORE "*restore"

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
  int nro_msg;
  char buf[DEFAULT_BUFLEN];
} message;

typedef struct
{
	int id;
	bool used;
	bool enabled;
    char *name;
    char socketAddress[14];
} Client;

typedef struct
{
    SOCKET socket;
	Client *clients;
    int Length;
} ClientList;
