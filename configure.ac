#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([invm-frameworks], [1.0.0.1051], [namratha.n.kothapalli@intel.com])
AC_CONFIG_SRCDIR([invm-cim/src/common/common_types.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_CANONICAL_HOST
build_windows=no
case "${host_os}" in
	linux*)
		build_linux=yes
	;;
	cygwin*|mingw*)
		build_windows=yes
	;;
esac
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])

AC_CHECK_HEADER(windows.h, AC_DEFINE(HAVE_WINDOWS_H, 1, [Compiling on Windows]))

AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability silent-rules tar-pax no-dist-gzip dist-xz subdir-objects parallel-tests])
AM_SILENT_RULES([yes])

LT_PREREQ(2.2)
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_CXX
AC_C_INLINE

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h stddef.h stdlib.h string.h sys/param.h unistd.h])
AC_CHECK_HEADERS([limits.h syslog.h wchar.h])
AC_CHECK_HEADERS([libintl.h sys/ioctl.h termios.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([getpagesize memset munmap])
AC_CHECK_FUNCS([memchr pow strcspn strtoull])

AC_CONFIG_FILES([Makefile invm-cim.pc invm-cli.pc invm-i18n.pc])
AC_OUTPUT
