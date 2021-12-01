#ifndef _STATUS_H_
#define _STATUS_H_

#define __EMPTY__

#define ERR_TEXT(s) status__get_text(s)

#define ASSERT_ARG(cond, ret) \
    if (!(cond)) {            \
        return ret;           \
    }

#define IFN_OK(s) if (s != BPSP_OK)

#define IF_OK(s) if (s == BPSP_OK)

#if defined(DEBUG)
#define ASSERT_BPSP_OK(s)        \
    IFN_OK(s) {                  \
        log__error(ERR_TEXT(s)); \
        return s;                \
    }
#else
#define ASSERT_BPSP_OK(s) IFN_OK(s) return s
#endif

typedef enum {

    BPSP_OK = 0,  ///< Success

    BPSP_ERR,              ///< Generic error
    BPSP_PROTOCOL_ERROR,   ///< Error when parsing a protocol message,
                           ///  or not getting the expected message.
    BPSP_IO_ERROR,         ///< IO Error (network communication).
    BPSP_MAX_VAR_HEADERS,  ///< The protocol message read from the socket
                           ///  does not fit in the read buffer.

    BPSP_CONNECTION_CLOSED,           ///< Operation on this connection failed because
                                      ///  the connection is closed.
    BPSP_NO_SERVER,                   ///< Unable to connect, the server could not be
                                      ///  reached or is not running.
    BPSP_STALE_CONNECTION,            ///< The server closed our connection because it
                                      ///  did not receive PINGs at the expected interval.
    BPSP_SECURE_CONNECTION_WANTED,    ///< The client is configured to use TLS, but the
                                      ///  server is not.
    BPSP_SECURE_CONNECTION_REQUIRED,  ///< The server expects a TLS connection.
    BPSP_CONNECTION_DISCONNECTED,     ///< The connection was disconnected. Depending on
                                      ///  the configuration, the connection may reconnect.

    BPSP_CONNECTION_AUTH_FAILED,  ///< The connection failed due to authentication error.
    BPSP_NOT_PERMITTED,           ///< The action is not permitted.
    BPSP_NOT_FOUND,               ///< An action could not complete because something
                                  ///  was not found. So far, this is an internal error.

    BPSP_ADDRESS_MISSING,  ///< Incorrect URL. For instance no host specified in
                           ///  the URL.

    BPSP_INVALID_OPCODE,       ///< An invalid opcode
    BPSP_INVALID_TOPIC,        ///< Invalid subject, for instance NULL or empty string.
    BPSP_INVALID_ARG,          ///< An invalid argument is passed to a function. For
                               ///  instance passing NULL to an API that does not
                               ///  accept this value.
    BPSP_NOT_COMPLETED_FRAME,  ///< The frame is not completed to operate on
    BPSP_INVALID_SUBSCRIBER,   ///< Invalid subsciber
    BPSP_TREE_DUP_SUBSCRIBER,  ///< Found duplicate subscriber in node_tree
    BPSP_INVALID_VAR_HEADERS,  ///< Invalid variable headers.

    BPSP_ILLEGAL_STATE,  ///< An unexpected state, for instance calling
                         ///  #natsSubscription_NextMsg() on an asynchronous
                         ///  subscriber.

    BPSP_SLOW_CONSUMER,  ///< The maximum number of messages waiting to be
                         ///  delivered has been reached. Messages are dropped.

    BPSP_MAX_PAYLOAD,         ///< Attempt to send a payload larger than the maximum
                              ///  allowed by the NATS Server.
    BPSP_MAX_DELIVERED_MSGS,  ///< Attempt to receive more messages than allowed, for
                              ///  instance because of #natsSubscription_AutoUnsubscribe().

    BPSP_INSUFFICIENT_BUFFER,  ///< A buffer is not large enough to accommodate the data.

    BPSP_NO_MEMORY,  ///< An operation could not complete because of insufficient
                     ///  memory.

    BPSP_SYS_ERROR,  ///< Some system function returned an error.

    BPSP_TIMEOUT,  ///< An operation timed-out. For instance
                   ///  #natsSubscription_NextMsg().

    BPSP_FAILED_TO_INITIALIZE,  ///< The library failed to initialize.
    BPSP_NOT_INITIALIZED,       ///< The library is not yet initialized.

    BPSP_SSL_ERROR,  ///< An SSL error occurred when trying to establish a
                     ///  connection.

    BPSP_NO_SERVER_SUPPORT,  ///< The server does not support this action.

    BPSP_NOT_YET_CONNECTED,  ///< A connection could not be immediately established and
                             ///  #natsOptions_SetRetryOnFailedConnect() specified
                             ///  a connected callback. The connect is retried asynchronously.

    BPSP_DRAINING,  ///< A connection and/or subscription entered the draining mode.
                    ///  Some operations will fail when in that mode.

    BPSP_INVALID_QUEUE_NAME,  ///< An invalid queue name was passed when creating a queue
                              ///< subscription.

    BPSP_NO_RESPONDERS,  ///< No responders were running when the server received the request.

    BPSP_MISMATCH,  ///< For JetStream subscriptions, it means that a consumer sequence mismatch was
                    ///< discovered.

    BPSP_MISSED_HEARTBEAT,  ///< For JetStream subscriptions, it means that the library detected that
                            ///< server heartbeats have been missed.

} status__err;

const char* status__get_text(status__err s);

#endif  // _STATUS_H_
