#ifndef _NET_H_
#define _NET_H_

#include <arpa/inet.h>

#include "datatype.h"
#include "status.h"

#define BPSP_NET_BACKLOG 20
#define BPSP_NET_BUFFER_SIZE 1024

typedef enum { NET_T_NOOP = -1, NET_T_PASSIVE = 0, NET_T_ACTIVE = 1 } net__type;

typedef enum {
    /** both **/
    NET_S_INIT = -1,
    NET_S_DISCONNECTED = 0,  ///< The connection has been disconnected
    NET_S_CLOSED,            ///< The connection is closed

    /** active connection  **/
    NET_S_CONNECTING,    ///< The connection is in the process or connecting
    NET_S_CONNECTED,     ///< The connection is connected
    NET_S_RECONNECTING,  ///< The connection is in the process or reconnecting

    /**  passive connection **/
    NET_S_LISTEN,

} net__state;

typedef struct {
    int sockfd;
    struct sockaddr_in* addr;

    net__type type;

    net__state state;

    /** internal **/
} bpsp__connection;

bpsp__connection* net__connect(const char* host, uint16_t port);
bpsp__connection* net__listen(const char* host, uint16_t port);
bpsp__connection* net__accept(bpsp__connection* listener);
bpsp__connection* net__dup(const bpsp__connection* conn);
void net__free(bpsp__connection* conn);
void net__destroy(bpsp__connection* conn);
status__err net__close(bpsp__connection* conn);
status__err net__read(bpsp__connection* conn, void* buf, size_t count, ssize_t* n_read, uint8_t block);
status__err net__write(bpsp__connection* conn, void* buf, size_t count, ssize_t* n_write, uint8_t block);

#endif  // _NET_H_
