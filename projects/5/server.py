import random
import socket
import sys
import threading
import time
from datetime import datetime, timedelta

import config


class Synchronizer(object):
    clients = set()
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
    def __init__(self, port=config.port, synchronizer=Synchronizer()):
        self.local_address = ('', int(port))

        # unwrap server_response_delay_range dividing it by 2, as simulate_delay is called twice,
        # simulating receiving delay and sending delay.
        self.min_response_delay, self.max_response_delay = [x / 2 for x in config.server_response_delay_range]

        self.synchronizer = synchronizer

        self.listening_worker = None

    def simulate_delay(self):
        time.sleep(random.random()
                   * (self.max_response_delay - self.min_response_delay)
                   + self.min_response_delay)

    def start(self):
        print('TimeServer is starting at %s...' % str(self.local_address))
        self._listen()
        print('bye')

    def _listen(self):
        listen_s = socket.socket(type=socket.SOCK_DGRAM)
        listen_s.bind(self.local_address)
        listen_s.settimeout(.1)

        while True:
            try:
                start = datetime.now()

                try:
                    data, address = listen_s.recvfrom(config.buffer)
                except socket.timeout:
                    continue

                data = str(data, encoding='utf-8')

                self.simulate_delay()

                elapsed = datetime.now() - start

                print('%s: %s.' % (str(address), data))

                if data == ':sync-cristian':
                    t = threading.Thread(target=self._cristian, args=(address,))
                elif data == ':sync-berkley':
                    t = threading.Thread(target=self._berkley, args=(address,))
                elif data == ':berkley-commit':
                    t = threading.Thread(target=self._berkley_commit, args=(address,))
                else:
                    continue

                t.daemon = True
                t.start()

            except KeyboardInterrupt:
                listen_s.close()
                return

    def _cristian(self, address):
        # If a client has requested cristian's algorithm, return server's current time.
        response = str(datetime.now())

        self.simulate_delay()

        s = socket.socket(type=socket.SOCK_DGRAM)
        s.bind(('', 0))
        s.sendto(bytes(response, encoding='utf8'), address)

        s.close()

    def _berkley(self, address):
        self.synchronizer.clients.add(address)

    def _berkley_commit(self, args):
        # If a client has requested berkley-commit, ask their time and add it to the synchronizer.
        s = socket.socket()
        s.bind(('', 0))
        s.settimeout(config.timeout)

        # Requests all clients' timestamps.
        for address in self.synchronizer.clients:
            print(':sync-berkley -> %s' % str(address))
            start = datetime.now()

            s.sendto(b':sync-berkley', address)
            data, address = s.recvfrom(config.buffer)
            print('%s sent us %s.' % (str(address), data))

            elapsed = datetime.now() - start

            remote_time = datetime.strptime(str(data, encoding='utf-8'), "%Y-%m-%d %H:%M:%S.%f")
            remote_time += elapsed / 2

            self.synchronizer.add(address, remote_time)

        offsets = self.synchronizer.offsets

        self.simulate_delay()

        for key, offset in offsets.items():
            data = bytes(str(offset), encoding='utf-8')
            address = self.synchronizer.address_from_key(key)

            s.sendto(data, address)

        self.synchronizer.clear()
        s.close()


if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else config.port

    TimeServer(port=port).start()
