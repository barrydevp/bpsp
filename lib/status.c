#include "status.h"

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

const char* status__get_text(status__err s) { return err_text[(int)s]; }
