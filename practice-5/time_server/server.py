import socket
import time
import random
from datetime import datetime

import config


class TimeServer(object):
    max_request_delay_in_seconds = 5 

    def __init__(self, connection=None, port=None, buffer_size=None):
        self.connection = connection or socket.socket(type=socket.SOCK_DGRAM)
        self.local_address = '', int(port or config.port)
        self.buffer_size = buffer_size or config.buffer_size

    def simulate_delay(self):    
        time.sleep(random.random() * self.max_request_delay_in_seconds / 2)

    def cristian(self, host_address):
        response = str(datetime.now())

        print('answer: %s\n' % response)

        self.simulate_delay() 
        
        self.connection.sendto(bytes(response, encoding='utf8'), host_address)
    
    def start(self):
        print('TimeServer is starting at %s\n' % str(self.local_address))

        self.connection.bind(self.local_address)
        self.connection.settimeout(.1)

        while True:
            try:
                try:
                    data, host_address = self.connection.recvfrom(self.buffer_size)
                    data = str(data, encoding='utf-8')

                    self.simulate_delay()
 
                    print('%s: %s' % (str(host_address), data))
  
                    if data == ':cristian':
                        self.cristian(host_address)

                except socket.timeout:
                    continue

            except KeyboardInterrupt:
                return self.stop()

    def stop(self):
        self.connection.close()
        print('Bye')

        return self
