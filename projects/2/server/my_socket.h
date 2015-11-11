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
	SOCKET socket;
	bool used;
	bool enabled;
} Client;

typedef struct
{
	Client *current;
	Client *clients;
} ClientList;