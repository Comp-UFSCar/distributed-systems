import re
import socket
import datetime

import config


def parseTimeDelta(s):
    """Create timedelta object representing time delta
       expressed in a string

    Takes a string in the format produced by calling str() on
    a python timedelta object and returns a timedelta instance
    that would produce that string.

    Acceptable formats are: "X days, HH:MM:SS" or "HH:MM:SS".

    P.s (lucas): function copied from
    <http://kbyanc.blogspot.com.br/2007/08/python-reconstructing-timedeltas-from.html>.
    """
    if s is None:
        return None

    d = re.match(r'((?P<days>\d+) days, )?(?P<hours>\d+):' r'(?P<minutes>\d+):(?P<seconds>\d+)', s).groupdict(0)
    return datetime.timedelta(**dict(((key, int(value)) for key, value in d.items())))


class TimeClient(object):
    def __init__(self, server_address, algorithm='cristian', connection=None):
        self.server_address = server_address
        self.buffer = config.buffer
        self.algorithm = algorithm

        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.connection.bind(('127.0.0.1', 0))

    def _cristian(self):
        start = datetime.datetime.now()

        self.connection.sendto(b':sync-cristian', self.server_address)
        data, address = self.connection.recvfrom(self.buffer)

        elapsed = datetime.datetime.now() - start

        # Parse remote time from string.
        remote_time = datetime.datetime.strptime(str(data, encoding='utf-8'), "%Y-%m-%d %H:%M:%S.%f")

        print('%s:%i: %s, taking %s.' % (address[0], address[1], data, str(elapsed)))

        # Calculate Cristian's algorithm result.
        return remote_time + elapsed / 2

    def _berkley(self):
        self.connection.sendto(b':sync-berkley', self.server_address)

        # The server answered with a new socket, its reference is stored in address.
        data, address = self.connection.recvfrom(self.buffer)
        print('%s: %s.' % (str(address), data))

        # Finally, send our current time.
        data = bytes(datetime.datetime.now(), encoding='utf-8')
        self.connection.sendto(data, address)

        data, address = self.connection.recvfrom(self.buffer)
        data = str(data, encoding='utf-8')

        print('%s -> %s.' % (str(address), data))

        offset_to_sync = parseTimeDelta(data)
        return offset_to_sync

    def start(self):
        print('%s request -> %s.' % (self.algorithm, str(self.server_address)))

        try:
            if self.algorithm == 'berkley-commit':
                self.connection.sendto(b':berkley-commit', self.server_address)
                return

            elif self.algorithm == 'cristian':
                synced_time = self._cristian()

            elif self.algorithm == 'berkley':
                offset_to_sync = self._berkley()

                synced_time = datetime.datetime.now() + offset_to_sync

            print('Synchronized time is %s.' % synced_time)

        except KeyboardInterrupt:
            pass

        return self.stop()

    def stop(self):
        if self.connection:
            self.connection.close()
            self.connection = None

        return self


if __name__ == '__main__':
    import argparse

    default_server_address = '127.0.0.1:' + str(config.port)

    parser = argparse.ArgumentParser(description='TimeClient')
    parser.add_argument('algorithm', help='The algorithm to be executed (cristian, berkley or berkley-commit)')
    parser.add_argument('--address', default=default_server_address,
                        help='The address of the time server (default: %s)' % default_server_address)

    args = parser.parse_args()

    address = args.address.split(':')
    address = address[0], int(address[1])

    TimeClient(address, args.algorithm).start()
