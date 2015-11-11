# Distributed systems - Practice 5

## Introduction
Implementation of Christian's and Berkley's algorithm.
 
* Project url: [https://github.com/lucasdavid/distributed-systems/practice-5](https://github.com/lucasdavid/distributed-systems/tree/master/practice-5).
* Student: Lucas Oliveira David, RA. 407917

## Executing
### Christian's algorithm
To execute Christian's algorithm, simply run the server and use a client to connect with it.
```shell
python server.py --port-number
python client.py cristian --time-server-address (default: 127.0.0.1:8980)
```

### Berkley's algorithm
To execute Berkley's algorithm, run the server and use multiple clients to connect with it.
Finally, instantiate a client with the `berkley-commit` argument to request the server to synchronize the clients.
```shell
python server.py --port-number

python client.py berkley --time-server-address (default: 127.0.0.1:8980)
python client.py berkley --time-server-address (default: 127.0.0.1:8980)
python client.py berkley --time-server-address (default: 127.0.0.1:8980)
...

python client.py berkley-commit --time-server-address (default: 127.0.0.1:8980)
```
