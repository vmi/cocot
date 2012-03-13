#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([cocot],[1.1-20120313])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([cocot.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-liconv':
AC_CHECK_LIB([iconv], [main])
AC_CHECK_LIB([util], [openpty],,
	[AC_CHECK_FILE([/dev/ptmx], [AC_CHECK_HEADERS([stropts.h])])])

# Checks for header files.
AC_CHECK_HEADERS([iconv.h],,[AC_MSG_ERROR(iconv.h is not available)])
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h sys/ioctl.h sys/time.h termios.h unistd.h utmp.h])
AC_CHECK_HEADERS([libutil.h pty.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memcpy memmove memset select strerror])
AC_CHECK_FUNCS([max cfmakeraw])
AC_CHECK_FUNCS([login_tty])

# DEFAULT_TERM_CODE
AC_ARG_WITH(default-term-code,
        [  --with-default-term-code=CODE	specify default value for term code (default is UTF-8)],
	[DEFAULT_TERM_CODE=$withval],
	[DEFAULT_TERM_CODE="UTF-8"])
AC_DEFINE_UNQUOTED(DEFAULT_TERM_CODE, "$DEFAULT_TERM_CODE", [default value for term code])

# DEFAULT_PROC_CODE
AC_ARG_WITH(default-proc-code,
        [  --with-default-proc-code=CODE	specify default value for proc code (default is CP932)],
	[DEFAULT_PROC_CODE=$withval],
	[DEFAULT_PROC_CODE="CP932"])
AC_DEFINE_UNQUOTED(DEFAULT_PROC_CODE, "$DEFAULT_PROC_CODE", [default value for proc code])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
