import random
import socket
import sys
import time
from datetime import datetime

import config


class TimeServer(object):
    def __init__(self, connection=None, port=config.port, buffer=config.buffer):
        self.local_address = ('', int(port))
        self.buffer = buffer

        # unwrap server_response_delay_range dividing it by 2, as simulate_delay is called twice,
        # simulating receiving delay and sending delay.
        self.min_response_delay, self.max_response_delay = [x / 2 for x in config.server_response_delay_range]

        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)

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
                try:
                    data, host_address = self.connection.recvfrom(self.buffer)
                except socket.timeout:
                    continue

                self.simulate_delay()

                response = str(datetime.now())
                print('Request from %s.' % str(host_address))

                self.simulate_delay()

                self.connection.sendto(bytes(response, encoding='utf8'), host_address)

            except KeyboardInterrupt:
                return self.stop()

    def stop(self):
        self.connection.close()
        print('Bye')

        return self

if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else config.port

    TimeServer(port=port).start()
