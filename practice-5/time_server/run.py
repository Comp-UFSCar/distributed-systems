import sys

from server import TimeServer

if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else None

    time_server = TimeServer(port=port)
    time_server.start()
