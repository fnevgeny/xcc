AC_PREREQ([2.71])

m4_include([version.m4])

AC_INIT([xcc],[XCC_VERSION_MAJOR.XCC_VERSION_MINOR.XCC_VERSION_NANO])
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR(libexe.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(1.7)
CFLAGS=${CFLAGS=}

AC_LANG(C)
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_DISABLE_SHARED
LT_INIT

# add /usr/local/lib and /usr/local/include
CPPFLAGS="$CPPFLAGS -I/usr/local/include"
LIBS="$LIBS -L/usr/local/lib"

AC_CHECK_LIB([expat], [XML_Parse], [], [AC_MSG_FAILURE([libexpat not found.])], [-L/usr/local/lib])

AC_CHECK_HEADERS(getopt.h stdarg.h string.h, , AC_MSG_ERROR([mandatory headers needed]))

dnl			AC_C_ADD_FLAG
AC_DEFUN([AC_C_ADD_FLAG],
    [AC_CACHE_CHECK([whether ${CC-cc} accepts $2], [ac_cv_c_flag_$1],
	    [saved_CFLAGS="$CFLAGS" ac_cv_c_flag_$1=no
		for i in $2; do
		    CFLAGS="$saved_CFLAGS $i"
		    # gcc-3.0 -ggdb crashes by this:
		    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define max(a, b)							\
	({								\
		typeof(a)	_a = (a);				\
		typeof(b)	_b = (b);				\
		MAX(_a, _b);						\
	})]], [[max(42, 63)]])],[ac_cv_c_flag_$1="$i"
			    break],[])
		done
		CFLAGS="$saved_CFLAGS"])
	if test "x$ac_cv_c_flag_$1" != "xno"; then
	    CFLAGS="$CFLAGS $ac_cv_c_flag_$1"
	fi])


AC_C_ADD_FLAG(wall, -Wall)
AC_C_ADD_FLAG(w, -W)
AC_C_ADD_FLAG(pedantic, -pedantic)
AC_C_ADD_FLAG(pointer_arith, -Wpointer-arith)
AC_C_ADD_FLAG(no_unused_parameter, -Wno-unused-parameter)
dnl AC_C_ADD_FLAG(wsign_compare, -Wsign-compare)
dnl AC_C_ADD_FLAG(wshadow, -Wshadow)
dnl AC_C_ADD_FLAG(wcast_qual, -Wcast-qual)
dnl AC_C_ADD_FLAG(wbad_function_cast, -Wbad-function-cast)
AC_C_ADD_FLAG(o, -O2)
AC_C_ADD_FLAG(g, -g)

MAINTAINERCLEANFILES=Makefile.in
AC_SUBST(MAINTAINERCLEANFILES)

AC_DEFINE_UNQUOTED(XCC_VERSION_STRING, "$PACKAGE_NAME-$PACKAGE_VERSION", [package version])

AC_SUBST(VERSION)
AC_CONFIG_FILES([Makefile							\
	  examples/Makefile						\
	  packaging/fedora/xcc.spec])
AC_OUTPUT

echo
echo
echo "configuration was successful"
echo "now type \`${MAKE-make}' and \`${MAKE-make} install'"
echo
echo
