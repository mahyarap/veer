#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([veer], [0.0.1])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/veer.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_SEARCH_LIBS([initscr], [curses])

# Checks for header files.
AC_CHECK_HEADERS([curses.h limits.h stddef.h stdlib.h string.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strerror strrchr])

AC_ARG_ENABLE(debug,
[  --enable-debug          Enable debugging (disabled by default)],
[if test x$enableval = xyes; then
    AC_DEFINE(DEBUG, 1, [Define this to enable debug messages and assert warnings.]) debug_support=yes
fi])

if test x$debug_support != xyes; then
    AC_DEFINE(NDEBUG, 1, [Shut up assert warnings :-)])
fi


AC_CONFIG_FILES([Makefile 
				 src/Makefile])

AC_OUTPUT

