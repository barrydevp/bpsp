#ifndef _STATUS_H_
#define _STATUS_H_

typedef enum {
  BPSP_CONN_STATUS_DISCONNECTED = 0,  ///< The connection has been disconnected
  BPSP_CONN_STATUS_CONNECTING,        ///< The connection is in the process or connecting
  BPSP_CONN_STATUS_CONNECTED,         ///< The connection is connected
  BPSP_CONN_STATUS_CLOSED,            ///< The connection is closed
  BPSP_CONN_STATUS_RECONNECTING,      ///< The connection is in the process or reconnecting
  BPSP_CONN_STATUS_DRAINING_SUBS,     ///< The connection is draining subscriptions
  BPSP_CONN_STATUS_DRAINING_PUBS,     ///< The connection is draining publishers
} status__conn;

typedef enum {

  BPSP_ERR_OK = 0,  ///< Success

  BPSP_ERR,             ///< Generic error
  BPSP_PROTOCOL_ERROR,  ///< Error when parsing a protocol message,
                        ///  or not getting the expected message.
  BPSP_IO_ERROR,        ///< IO Error (network communication).
  BPSP_LINE_TOO_LONG,   ///< The protocol message read from the socket
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

  BPSP_INVALID_SUBJECT,       ///< Invalid subject, for instance NULL or empty string.
  BPSP_INVALID_ARG,           ///< An invalid argument is passed to a function. For
                              ///  instance passing NULL to an API that does not
                              ///  accept this value.
  BPSP_INVALID_SUBSCRIPTION,  ///< The call to a subscription function fails because
                              ///  the subscription has previously been closed.
  BPSP_INVALID_TIMEOUT,       ///< Timeout must be positive numbers.

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

static const char* err_text[] = {
    "OK",

    "Error",
    "Protocol Error",
    "IO Error",
    "Line too long",

    "Connection Closed",
    "No server available for connection",
    "Stale Connection",
    "Secure Connection not available",
    "Secure Connection Required",
    "Connection Disconnected",
    "Authentication Violation",

    "Not Permitted",
    "Not Found",

    "TCP Address missing",

    "Invalid Subject",
    "Invalid Argument",
    "Invalid Subscription",
    "Invalid Timeout",

    "Illegal State",

    "Slow Consumer, messages dropped",

    "Maximum Payload Exceeded",
    "Maximum Messages Delivered",

    "Insufficient Buffer",

    "No Memory",

    "System Error",

    "Timeout",

    "Initialization Failed",
    "Not Initialized",

    "SSL Error",

    "Not Supported By Server",

    "Not Yet Connected",

    "Draining in progress",

    "Invalid queue name",

    "No responders available for request",

    "Mismatch",
    "Missed Server Heartbeat",
};

const char* status__get_text(status__conn s) { return err_text[(int)s]; }

#endif  // _STATUS_H_
