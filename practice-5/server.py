import random
import socket
import sys
import time
from datetime import datetime, timedelta

import config


class Synchronizer(object):
    differences = dict()

    @staticmethod
    def key_from_address(address):
        return ':'.join([str(x) for x in address])

    @staticmethod
    def address_from_key(key):
        fragments = key.split(':')

        return fragments[0], int(fragments[1])

    def has_address(self, address):
        return self.key_from_address(address) in self.differences

    def add(self, address, remote_time):
        """Add reference to a remote client time to the list.
        """
        if not self.has_address(address):
            difference = remote_time - datetime.now()

            key = self.key_from_address(address)

            self.differences[key] = difference

        return self

    def clear(self):
        self.differences = dict()
        return self

    @property
    def offsets(self):
        average_difference = self.average_difference

        offsets = {key: difference - average_difference for key, difference in self.differences.items()}

        return offsets

    @property
    def average_difference(self):
        differences = list(self.differences.values())
        return sum(differences, timedelta()) / len(differences)

    @property
    def average_time(self):
        return datetime.now() + self.average_difference()


class TimeServer(object):
    def __init__(self, port=config.port, buffer=config.buffer, timeout=config.timeout,
                 connection=socket.socket(type=socket.SOCK_DGRAM), synchronizer=Synchronizer()):
        self.local_address = ('', int(port))
        self.buffer = buffer

        self.timeout = timeout

        # unwrap server_response_delay_range dividing it by 2, as simulate_delay is called twice,
        # simulating receiving delay and sending delay.
        self.min_response_delay, self.max_response_delay = [x / 2 for x in config.server_response_delay_range]

        self.synchronizer = synchronizer
        self.connection = connection

    def simulate_delay(self):
        time.sleep(random.random()
                   * (self.max_response_delay - self.min_response_delay)
                   + self.min_response_delay)

    def start(self):
        print('TimeServer is starting at %s...' % str(self.local_address))

        self.connection.bind(self.local_address)
        self.connection.settimeout(.1)

        while True:
            try:
                start = datetime.now()

                try:
                    data, address = self.connection.recvfrom(self.buffer)
                    data = str(data, encoding='utf-8')
                except socket.timeout:
                    continue

                self.simulate_delay()

                elapsed = datetime.now() - start

                print('%s: %s.' % (str(address), data))

                if data == ':sync-cristian':
                    self._cristian(address)
                elif data == ':sync-berkley':
                    self._berkley_step_2(address)
                else:
                    self._berkley_step_1(address, data, elapsed)

            except KeyboardInterrupt:
                return self.stop()

    def _cristian(self, address):
        # If a client has requested cristian's algorithm, return server's current time.
        response = str(datetime.now())

        self.simulate_delay()
        self.connection.sendto(bytes(response, encoding='utf8'), address)

    def _berkley_step_1(self, address, data, elapsed):
        # If a client has sent their time, add it to the synchronizer, using cristian's algorithm to amend the delay.
        remote_time = datetime.strptime(data, "%Y-%m-%d %H:%M:%S.%f")
        remote_time += elapsed / 2

        self.synchronizer.add(address, remote_time)

    def _berkley_step_2(self, address):
        # If a client has requested berkley's algorithm, ask their time.
        offsets = self.synchronizer.offsets

        self.simulate_delay()

        for key, offset in offsets.items():
            data = bytes(str(offset), encoding='utf-8')
            address = self.synchronizer.address_from_key(key)

            self.connection.sendto(data, address)

        self.synchronizer.clear()

    def stop(self):
        self.connection.close()
        print('Bye')

        return self

if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else config.port

    TimeServer(port=port).start()
