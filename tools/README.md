The BPSP Command Line Interface Tool
=================

A command line interface tool to interact with BPSP broker.

## Installing

## Usage

Using bpsp cli tool to publish and subscribe to topics.

### Help

```
$ bpsp --help

Usage: bpsp <options> command [arguments...]
  command:
    sub <topics...>        Subscribe on topics
    pub <topics...>        Publish on topics

  options:
    -h, --host             Broker host address
    -p, --port             Broker port address
    -l, --logs             Show logs
    -t, --timestamps       Show logs timestamps
    -v, --version          Print version details
    -?, --help             Print usage

Basic Publish Subscribe Protocol CLI Tools
Written by barrydevp(barrydevp@gmail.com)

```

### Subscribe 

#### Subscribe single topic

We will subscribe to `locationA/sensorA` topic:

```
$ bpsp sub locationA/sensorA

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/sensorA .

```

We will subscribe to `locationA/*` wildcard topic:

```
$ bpsp sub "locationA/*"

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/* .

```

#### Subscribe multiple topic

```
$ bpsp sub locationA/sensorA "locationB/+" "locationC/*"

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/sensorA .
Subscribing on locationB/+ .
Subscribing on locationC/* .

```

### Publish

#### Subscribe single topic

We will publish to `locationA/sensorA` topic:

```
$ bpsp pub locationA/sensorA

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

enter: hello world
Published 11 bytes to "locationA/sensorA" .
enter:

```

The receiver will show:

```
$ bpsp sub locationA/sensorA

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/sensorA .
127.0.0.1:33898 ->> "locationA/sensorA" Received 11 bytes : hello world

```

#### Subscribe multiple topic

We will publish to `locationA/sensorA` topic:

```
$ bpsp pub locationA/sensorA locationB/sensorB locaionC/sensorC

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

enter: hello world
Published 11 bytes to "locationA/sensorA" .
Published 11 bytes to "locationB/sensorB" .
Published 11 bytes to "locationC/sensorC" .
enter:

```

The receiver will show:

```
$ bpsp sub locationA/sensorA "locationB/+" "locationC/*"

Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Subscribing on locationA/sensorA .
Subscribing on locationB/+ .
Subscribing on locationC/* .
127.0.0.1:33906 ->> "locationA/sensorA" Received 11 bytes : hello wrold
127.0.0.1:33906 ->> "locationB/+" Received 11 bytes : hello wrold
127.0.0.1:33906 ->> "locationC/*" Received 11 bytes : hello wrold

```

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

After building, the `bpsp` binary file in current directory is the bpsp cli tool

## Credits

BPSP CLI tool was written by Hai Dao - barrydevp <barrydevp@gmail.com>
