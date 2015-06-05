import sys

from client import TimeClient

if __name__ == '__main__':
    address = sys.argv[1]

    ip, port = address.split(':')

    TimeClient((ip, int(port))) \
        .start()
