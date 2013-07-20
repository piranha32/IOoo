#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([IOoo], [0.1], [james.zapico@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

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
# First check for C++11 support
AX_CXX_COMPILE_STDCXX_11
# AC_COMPILE_IFELSE([AC_LANG_SOURCE(
#   [[template <typename T>
#     struct check
#     {
#       static_assert(sizeof(int) <= sizeof(T), "not big enough");
#     };
# 
#     typedef check<check<bool>> right_angle_brackets;
# 
#     int a;
#     decltype(a) b;
# 
#     typedef check<int> check_type;
#     check_type c;
#     check_type&& cr = static_cast<check_type&&>(c);]])],,
#   AC_MSG_FAILURE(['$CXX $CXXFLAGS' does not accept ISO C++11]))

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
