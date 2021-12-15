#ifndef _LOG_H_
#define _LOG_H_

#include <errno.h>
#include <stdint.h>

/**
 * logging with timestamp with format like this
 *
 * print label and timestamp eg: " 2021-10-13T15:05:04.841Z    INFO] msg "
 */

/** log options **/
extern int log__timestamps;
extern int log__stack_trace;
extern int log__enable;
extern int log__frame;

void _log__print(const char* label, const char* fmt, ...);
void _log__info(const char* fmt, ...);
void _log__error(const char* fmt, ...);
void _log__debug(const char* fmt, ...);
void _log__warn(const char* fmt, ...);

#define log__stack _log__print("Stack", "%s at: %s (in %s:%d)", strerror(errno), __FUNCTION__, __FILE__, __LINE__)

#define log__print(label, fmt, ...) _log__print(label, fmt, ##__VA_ARGS__)

#define log__info(fmt, ...) _log__print("INFO", fmt, ##__VA_ARGS__)

#define log__debug(fmt, ...) _log__print("DEBUG", fmt, ##__VA_ARGS__)

#define log__warn(fmt, ...)                  \
    _log__print("WARN", fmt, ##__VA_ARGS__); \
    if (log__stack_trace) log__stack

#define log__error(fmt, ...)                  \
    _log__print("ERROR", fmt, ##__VA_ARGS__); \
    if (log__stack_trace) log__stack

#define log__trace(label, client, frame, fmt, ...)                                                                    \
    _log__print("TRACE", label " %-s. %s:%d:%s" fmt, OP_TEXT(frame->opcode), inet_ntoa(client->conn->addr->sin_addr), \
                ntohs(client->conn->addr->sin_port), client->_id, ##__VA_ARGS__)

#define log__trace_in(client, frame) log__trace("<<-", client, frame, "")

#define log__trace_out(client, frame) log__trace("->>", client, frame, "")

#endif  // _LOG_H_
