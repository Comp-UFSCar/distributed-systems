import socket

import config
import threading
import time
from datetime import datetime


class TimeClient(object):
    def __init__(self, server_address, connection=None):
        self.server_address = server_address
        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.buffer_length = config.buffer_length

    def start(self):
        print('Client is starting!')

        start = datetime.now()

        try:
            self.connection.sendto(b':cristian', self.server_address)
            data, address = self.connection.recvfrom(self.buffer_length)

            elapsed = datetime.now() - start

            server_time = datetime.strptime(str(data, encoding='utf8'), '%Y-%m-%d %H:%M:%S.%f') + elapsed / 2

            print('%s: %s' % (str(address), data))
            print('Elapsed: %s, estimated server time: %s\n' % (elapsed, server_time))

            self.connection.close()

        except KeyboardInterrupt:
            pass
 
        return self
