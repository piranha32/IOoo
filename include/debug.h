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
	// This used to use ##__VA_ARGS__, but this is classed as too ambiguous in G++11
	#define iooo_debugfva(level, format, ...) { \
		if(level <= DEBUG_LEVEL){ \
			printf("%s:%i:"#format"\n", __FILE__, __LINE__, __VA_ARGS__); \
		} \
	}

	#define iooo_debugf(level, format) iooo_debug(level, format, NULL)

	// Manual sneaky macro overloading
	#define GET_DEBUG(_1, _2, _3, NAME, ...) NAME
	#define iooo_debug(...) GET_DEBUG(__VA_ARGS__, iooo_debugfva, iooo_debugf)(__VA_ARGS__)
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
