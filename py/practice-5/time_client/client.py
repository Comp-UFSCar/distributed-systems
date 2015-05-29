import socket

import config
import threading
import time


class TimeClient(object):
    def __init__(self, server_address, connection=None):
        self.server_address = server_address
        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.buffer_length = config.buffer_length

    def start(self):
        print('Client is starting!')

        try:
            while True:
                time.sleep(2)

                self.connection.sendto(b'hellooooooo', self.server_address)
                data, address = self.connection.recvfrom(self.buffer_length)

                print('%s: %s.' % (data, str(address)))

        except KeyboardInterrupt:
            return self.stop()

    def stop(self):
        self.connection.close()

        return self
