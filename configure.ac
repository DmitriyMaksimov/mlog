AC_PREREQ(2.6)
AC_INIT([mlog], [1.1.0], [mike_n@hotbox.ru])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2 -Wall -Werror foreign])

AC_PROG_CXX
AX_BOOST_BASE([1.35])
if test "x$detected_boost_version" == "x" ; then
    AC_MSG_ERROR([[boost libraries are required to build]])
    exit 1
fi
AX_BOOST_THREAD
AC_CHECK_HEADERS([locale.h stdlib.h sys/time.h sys/timeb.h process.h])
AC_CHECK_FUNCS([setlocale gettimeofday localtime_r setlocale])
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

if test "x$build_os" = "xming32" ; then
    exec_charset="CP1251"
elif test "x$build_os" = "xcygwin" ; then
    exec_charset="CP1251"
else
    exec_charset="UTF-8"
fi
echo Using execution charset $exec_charset
AC_SUBST(exec_charset)

AC_CONFIG_FILES([
    Makefile
    examples/Makefile
])

AC_OUTPUT
