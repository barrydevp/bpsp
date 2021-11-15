#ifndef _LOG_H_
#define _LOG_H_

/**
 * logging with timestamp with format like this
 *
 * print label and timestamp eg: " 2021-10-13T15:05:04.841Z    INFO] msg "
 */

#define log__trace _log__print("Trace", "at: %s (in %s:%d)", __FUNCTION__, __FILE__, __LINE__)

#define log__print(label, fmt, ...) _log__print(label, fmt, ##__VA_ARGS__)

#define log__info(fmt, ...) _log__print("INFO", fmt, ##__VA_ARGS__)

#define log__debug(fmt, ...)       \
    _log__print("DEBUG", fmt, ##__VA_ARGS__); \
    log__trace

#define log__warn(fmt, ...)       \
    _log__print("WARN", ##__VA_ARGS__); \

#define log__error(fmt, ...)       \
    _log__print("ERROR", fmt, ##__VA_ARGS__); \
    log__trace

void _log__print(const char* label, const char* fmt, ...);
void _log__info(const char* fmt, ...);
void _log__error(const char* fmt, ...);
void _log__debug(const char* fmt, ...);
void _log__warn(const char* fmt, ...);

#endif  // _LOG_H_
