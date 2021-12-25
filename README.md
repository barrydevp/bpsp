Basic Publish Subscribe Protocol (BPSP)
=================
Họ và tên	      Công việc
Đào Minh Hải	  Thiết kế giao thức + Broker + Base Subscriber UI java
Hà Văn Hoài	    Core Client Java(xử lý gửi nhận Frame) + Subscriber
Đào Đình Công	  Auto gen data publisher C client + Publisher java
Phạm Xuân Hanh	Publisher UI java + docs


BPSP is light weight publish subscribe protocol. The project contain the [specification](https://github.com/barrydevp/bpsp/tree/master/specs/specification.md)
and the source code implementation of a client-server for version 1.0.

## Installing

## Quick start

After install the binary broker server, it can be started with basic configuration:

```
$ bpsp_server

  INFO] Creating broker listen on 0.0.0.0:29010
  INFO] Binding...
  INFO] Listen...
  INFO] Broker listening on 0.0.0.0:29010 !

```

The borker will listen on all available network interfaces on your machine.
Then use [bpsp cli tool](https://github.com/barrydevp/bpsp/tree/master/tools/README.md) to interacting with broker:

```
$ bpsp sub locationA/sensorA

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/sensorA .
127.0.0.1:33912 ->> "locationA/sensorA" Received 11 bytes : hello there

$ bpsp pub locationA/sensorA

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

enter: hello there
Published 11 bytes to "locationA/sensorA" .
enter:

```

## Documentation

## Building from source

### Dependencies

- [CMake v3.12 or later](https://cmake.org/download/)
- A C compiler (GCC 4.4+, clang, and MSVC are test. Other compilers may also work)
- pthreads
- [uthash / utlist](http://troydhanson.github.io/uthash)

### Building

First, download the source code:
```
$ git clone https://github.com/barrydevp/bpsp .
```

To build the library, use [CMake](https://cmake.org/download/). Make sure that CMake is added in your path, to verify run:
```
$ cmake --version

cmake version 3.21.3

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

Create a `build` directory (any name you like) from the root source code, and `cd` into it. Then run the following command once for all:
```
$ mkdir build && cd build
$ cmake ..
$ cmake --build . [--config Release]
```

After building, the `server` binary file in current directory is the output broker server program.

### Testing

After building, run test command:
```
$ ctest
```

## Development

### TODO

### Coding convention

Exporting something need prefix with file name. eg: file name `status.c` export function `get_text()` will result in `status__get_text()`.
Unexported something ignore this prefix.

## Links

## Credits

BPSP was written by Hai Dao - barrydevp <barrydevp@gmail.com>
