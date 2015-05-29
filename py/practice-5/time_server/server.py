import socket
from datetime import datetime

import config


class TimeServer(object):
    def __init__(self, connection=None, port=None, buffer_size=None):
        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.local_address = '', int(port or config.port)
        self.buffer_size = buffer_size or config.buffer_size

    def start(self):
        print('TimeServer is starting at %s...' % str(self.local_address))

        self.connection.bind(self.local_address)
        self.connection.settimeout(.1)

        while True:
            try:
                try:
                    data, host_address = self.connection.recvfrom(self.buffer_size)
                except socket.timeout:
                    continue

                response = str(datetime.now())
                print('Responding %s to %s.' % (response, str(host_address)))

                self.connection.sendto(response, host_address)

            except KeyboardInterrupt:
                return self.stop()

    def stop(self):
        self.connection.close()
        print('Bye')

        return self
