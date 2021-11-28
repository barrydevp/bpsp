Basic Publish Subscribe Protocol (BPSP) Java Client
=================

BPSP is light weight publish subscribe protocol. The project contain the [specification](https://github.com/barrydevp/bpsp/blob/master/specs/specification.md). This directory is source code implementation for java client of BPSP.

## Installing

## Quick start

## Documentation

## Building from source

### Dependencies

- JDK 11
- [Apache Maven](https://maven.apache.org/download.cgi), you can install this on linux with `apt`:
```
sudo apt update
sudo apt install maven
```

### Building

First, download the source code:
```
$ git clone https://github.com/barrydevp/bpsp .
```

To build the library, use [Maven](https://maven.apache.org/download.cgi). Make sure that Maven is added in your path, to verify run:
```
$ mvn --version
```

Then `cd` into client/java-client directory, and run the following command:
```
$ cd client/java-client
$ mvn compile
```

After building, you can run the client app by:
```
$ mvn exec:java -Dexec.mainClass=com.App
```

### Testing

## Development

### TODO

### Coding convention

## Links

## Credits

BPSP Java Client was written by Ha Hoai - abcvjp <hahoai1301@gmail.com>