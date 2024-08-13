# nGate

nGate is a simple reverse proxy written in C. It allows you to configure proxy rules using a configuration file specified by the -f flag. nGate is designed primarily to be straightforward and simple to understand.

## Installation

nGate uses CMake. To install nGate, clone the repository and build the program:
```
git clone https://github.com/yourusername/ngate.git
cd ngate
mkdir bin && cd bin
cmake ..
cmake --build .
```

## Usage

To run nGate, use the -f flag to specify your configuration file:

```
./ngate -f path/to/config/file
```

## Configuration

The configuration file specifies the proxy rules. Here is an example configuration:

```
port = 80
proxy_port = 3000
```

- port = [port] - The port nGate will listen on.
- proxy_port = [port] - The backend server to proxy requests to.

## Future
I plan to add the following features:
- Load balancing
- SSL connections
- Response caching

## License

This project is licensed under the MIT License. See the LICENSE file for details.
