#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(ezsh, 1.0, viviansnow@bupt.edu.cn)
AC_CONFIG_SRCDIR([src/readline_and_history.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(ezsh, 1.0)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(readline, readline , [], [
        echo "Error! You need to have libreadline installed!"
        exit -1
        ])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h termio.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 setenv strerror strncasecmp strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
