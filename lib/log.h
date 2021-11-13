#ifndef _LOG_H_
#define _LOG_H_

/**
 * logging with timestamp with format like this
 *
 * print label and timestamp eg: " 2021-10-13T15:05:04.841Z    INFO] msg "
 */

void log__print(const char* label, const char* fmt, ...);
void log__info(const char* fmt, ...);
void log__error(const char* fmt, ...);
void log__debug(const char* fmt, ...);
void log__warn(const char* fmt, ...);

#endif // _LOG_H_

