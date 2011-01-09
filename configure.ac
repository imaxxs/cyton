dnl Process this file with autoconf to create configure.

AC_INIT([cyton],[0.1],[imax@cc.gatech.edu])

AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_AUX_DIR(config)
dnl AM_INIT_AUTOMAKE(0.1)
AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE
dnl CERCS_REQUIRE_PACKAGE(pbio, io.h, libIO.la)
AC_CHECK_FUNCS(atol,,AC_MSG_ERROR(oops! no atol ?!?!?!?!?))
AC_CHECK_FUNCS(atoi,,AC_MSG_ERROR(oops! no atoi ?!?!?!?!?))
AC_HEADER_STDC

if test -n "${CFLAGS}"; then
	user_set_cflags=yes
fi
AC_PROG_CC
if test X$user_set_cflags != Xyes; then
	CFLAGS="-g"
fi
if test -n "${CXXFLAGS}"; then
	user_set_cxxflags=yes
fi
AC_PROG_CXX
if test X$user_set_cxxflags != Xyes; then
	CXXFLAGS="-g"
fi
AC_PROG_INSTALL
AC_PROG_LIBTOOL

LT_CONFIG_LTDL_DIR([libltdl])
LTDL_INIT
AC_CONFIG_AUX_DIR([libltdl/config])
AC_CONFIG_MACRO_DIR([libltdl/m4])
CERCS_REQUIRE_PACKAGE(ffs, ffs.h, libffs.la)
CERCS_REQUIRE_PACKAGE(gen_thread, gen_thread.h,libgen_thread.la)
CERCS_REQUIRE_PACKAGE(atl,atl.h,libatl.la)
CERCS_REQUIRE_PACKAGE(evpath,evpath.h,libevpath.la)

AM_INIT_AUTOMAKE(0.1)
VERSION="0.1"
AC_SUBST(VERSION)

AC_OUTPUT(Makefile util/Makefile context/Makefile formats/Makefile graph/Makefile metrics/Makefile monitoring/Makefile planner/Makefile bootserver/Makefile agent/Makefile functions/Makefile) 
