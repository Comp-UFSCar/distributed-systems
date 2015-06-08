import socket
import time
import sys
from datetime import datetime

import config


class TimeClient(object):
    def __init__(self, server_address, connection=None):
        self.server_address = server_address
        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.buffer = config.buffer

    def start(self):
        print('Client is about to request time from %s.' % self.server_address)

        try:
            self.connection.sendto(bytes(str(datetime.now())), self.server_address)
            data, address = self.connection.recvfrom(self.buffer)

            print('%s: %s.' % (data, str(address)))

        except KeyboardInterrupt:
            self.connection.close()

        return self


if __name__ == '__main__':
    address = sys.argv[1].split(':') if len(sys.argv) > 1 else '127.0.0.1', config.port

    TimeClient(address).start()
