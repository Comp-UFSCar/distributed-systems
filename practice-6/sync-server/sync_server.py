import socket

import config


class ResourceManager(object):
    map = {}

    def is_locked(self, resource):
        """Check if a resource is being held.
        """
        return resource in self.map and self.map[resource]

    def peak_holder(self, resource):
        """Peak who is the holder of a given resource.
        """
        try:
            return self.map[resource][0]
        except (IndexError, KeyError):
            return None

    def lock(self, resource, origin):
        """Lock a resource :resource to a holder :origin.

        Try to lock a resource to a holder :origin. If the resource is already held by someone else,
        add :origin to the list of holders.
        """
        resource_is_available = False

        if not self.is_locked(resource):
            self.map[resource] = []
            resource_is_available = True

        # Add origin to list of holders, regardless.
        self.map[resource].append(origin)

        return resource_is_available

    def unlock(self, resource, origin):
        """Unlock a resource from a holder.

        Unlock the resource :resource from a holder :origin. Prevents all unlocking requests from all origins that
        are not the holder (the first in the holders list).
        """
        try:
            # prevents unlock operation, unless the origin is the current resource's holder.
            if origin != self.peak_holder(resource):
                return False

            self.map[resource].remove(origin)
            return True

        except (IndexError, KeyError, ValueError):
            # Ignore cases where the resource isn't allocated
            # or the origin isn't a requester of that resource.
            return False


class SyncServer(object):
    _s = _resource_manager = None
    enabled = True

    @property
    def resource_manager(self):
        if not self._resource_manager:
            self._resource_manager = ResourceManager()

        return self._resource_manager

    @property
    def local_address(self):
        return socket.gethostbyname(socket.gethostname()), config.sync_server_port

    def start(self):
        try:
            print('Sync-server starting at (%s:%i)...' % self.local_address)
            self._listen()
        except (KeyboardInterrupt, EOFError):
            self.stop()

    def stop(self):
        if self._s:
            self._s.close()
            print('bye')

    def _listen(self):
        self._s = socket.socket(type=socket.SOCK_DGRAM)

        self._s.bind(('', config.sync_server_port))
        self._s.settimeout(.1)

        while True:
            try:
                data, address = self._s.recvfrom(config.buffer_length)
                data = str(data, encoding='utf-8')

                print('%s:%i: %s' % (address[0], address[1], data))

                if not self.enabled:
                    continue

                fragments = data.split()

                action = fragments[0]
                resource = fragments[1]

                if action == 'lock':
                    resource_is_available = self.resource_manager.lock(resource, address)

                    if resource_is_available:
                        self._s.sendto(bytes(resource, encoding='utf-8'), address)

                elif action == 'unlock':
                    self.resource_manager.unlock(resource, address)

                    # send message to the next requester in line.
                    next_holder = self.resource_manager.peak_holder(resource)
                    if next_holder:
                        self._s.sendto(bytes(resource, encoding='utf-8'), next_holder)

            except socket.timeout:
                # ignore timeouts. This is only here so sync-server will listen to INT signals.
                continue


if __name__ == '__main__':
    SyncServer().start()
