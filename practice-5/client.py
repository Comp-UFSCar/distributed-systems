import socket
import time
import sys
from datetime import datetime

import config


class TimeClient(object):
    def __init__(self, server_address, algorithm='cristian', connection=None):
        self.server_address = server_address
        self.buffer = config.buffer
        self.algorithm = algorithm

        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)

    def _cristian(self, data, address, elapsed):
        data = str(data, encoding='utf-8')

        return datetime.strptime(data, "%Y-%m-%d %H:%M:%S.%f") + elapsed / 2

    def _berkley(self, data, address, elapsed):
        data = str(data, encoding='utf-8')

        return datetime.strptime(data, "%Y-%m-%d %H:%M:%S.%f")

    def start(self):
        print('Client is about to request time from %s.' % str(self.server_address))

        try:
            start = datetime.now()

            local_time = datetime.now()

            self.connection.sendto(bytes(str(local_time), encoding='utf-8'), self.server_address)
            data, address = self.connection.recvfrom(self.buffer)

            elapsed = datetime.now() - start

            print('%s: %s, taking %s.' % (':'.join([str(s) for s in address]), data, str(elapsed)))

            if self.algorithm == 'cristian':
                sync_time = self._cristian(data, address, elapsed)
            elif self.algorithm == 'berkley':
                sync_time = self._berkley(data, address, elapsed)

            print('Synchronized time is %s.' % sync_time)

        except KeyboardInterrupt:
            self.connection.close()

        return self


if __name__ == '__main__':
    import argparse

    default_server_address = '127.0.0.1:' + str(config.port)

    parser = argparse.ArgumentParser(description='TimeClient')
    parser.add_argument('algorithm', help='The algorithm to be executed (cristian or berkley)')
    parser.add_argument('--address', default=default_server_address,
                        help='The address of the time server (default: %s)' % default_server_address)

    args = parser.parse_args()

    address = args.address.split(':')
    address = address[0], int(address[1])

    TimeClient(address, args.algorithm).start()
