#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([IOoo], [0.1], [james.zapico@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Allow for non-standard DTC compiler to be supplied
AC_SUBST([DTC])

AC_ARG_WITH(dtc,
    AS_HELP_STRING([--with-dtc=/path/to/dtc],
		[compiler for .dts files - must be compatible with overlay (-@) option]),
    [DTC="$withval"],
    [DTC=dtc]
)

# Kernel version setting
AC_SUBST([KERNEL_VERSION])

AC_ARG_WITH(kernel-version,
    AS_HELP_STRING([--with-kernel-version=4.x],
		[set the kernel version to be compatible with - affects the numbering of I2C buses]),
    [KERNEL_VERSION="$withval"],
    [KERNEL_VERSION=4.x]
)

# Debugging option --enable-debug[=level]
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
	[case "${enableval}" in
             yes)		debug=3 ;;
             no)		debug=0 ;;
             [[0-6]])	debug=${enableval} ;;
             *)			AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	esac],
	[debug=0])

AC_SUBST(DEBUG_LEVEL, "$debug")

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_MKDIR_P
AC_PROG_RANLIB
AM_PROG_AR

AC_LANG_PUSH([C++])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11

AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strerror])

AM_INIT_AUTOMAKE([-Wall foreign])


AC_CONFIG_FILES([
 Makefile
 src/Makefile
 examples/Makefile
 dts/Makefile
 include/Makefile
])

AC_OUTPUT
