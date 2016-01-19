/*
 * debug.h
 *
 *  Created on: Jun 20, 2013
 *      Author: jacek, chris
 */

#ifndef IOOO_DEBUG_H_
#define IOOO_DEBUG_H_

#include <stdio.h>
#include <errno.h>

#ifndef IOOO_DEBUG_LEVEL
#define IOOO_DEBUG_LEVEL 0
#endif

#ifndef IOOO_ERROR_LEVEL
#define IOOO_ERROR_LEVEL 1
#endif

/**
 * Debug macro
 * Params:
 *   @param level Level of detail in debug message
 *   @param format Formatting string for the debug message
 *   @param ... Format arguments
 */
#if IOOO_DEBUG_LEVEL > 0
	#include <stdarg.h>
	#include <string.h>

	inline void iooo_debug(const char* file, int line, int level, const char* format, ...)
	{
		if (level <= IOOO_DEBUG_LEVEL)
		{
			va_list args;
			va_start(args, format);
			fprintf(stdout, "%s:%i:", strrchr(file, '/') + 1, line);
			vfprintf(stdout, format, args);
			va_end(args);
		}
	}

	#define iooo_debug(...) iooo_debug(__FILE__, __LINE__, __VA_ARGS__)
#else
	#define iooo_debug(...) ((void) 0)
#endif

/**
 * Error macro. Maintains errno.
 * Params:
 *   @param ... Formatting string for error message,
 *   			followed by format arguments
 */
#if IOOO_ERROR_LEVEL > 0
	#define iooo_error(...) { \
		int errnobkp = errno; \
		fprintf(stderr, __VA_ARGS__); \
		errno = errnobkp; \
	}
#else
	#define iooo_error(...) ((void) 0)
#endif

#endif /* IOOO_DEBUG_H_ */
