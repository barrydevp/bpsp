Basic Publish Subscribe Protocol (BPSP)
=================

BPSP is light weight publish subscribe protocol. The project contain the [specification](https://github.com/barrydevp/bpsp/blob/master/specs/specification.md)
and the source code implementation of a client-server for version 1.0.

## Installing

## Quick start

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
