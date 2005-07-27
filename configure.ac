dnl Process this file with autoconf to produce a configure script.
AC_INIT(Lemur, 4.0, project@lemurproject.org)
AC_CONFIG_SRCDIR(app/src/RetEval.cpp)
AC_CONFIG_FILES(MakeDefns Makefile Makefile.app)
dnl To use a header file instead of command line -D defs, use
dnl AC_CONFIG_HEADERS(config.h)

dnl canonical host
AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_INSTALL
AC_CHECK_PROGS(AR, ar gar)

dnl Checks for libraries.
AC_CHECK_LIB(m, log)
AC_CHECK_LIB(pthread, pthread_create)
case $host in
   *-*-solaris*) 
     AC_CHECK_LIB(nsl, gethostbyname)
     AC_CHECK_LIB(socket, socket)
     ;;
   *linux*) 
     ;;
   *);;
esac
AC_CHECK_LIB(z, deflate)
AC_CHECK_LIB(iberty, mkstemps)

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_BIGENDIAN
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

dnl Checks for library functions.

AC_CHECK_FUNCS(mkstemp)
AC_CHECK_FUNCS(mkstemps)
dnl Finish up and output.
AC_SUBST(MODULES)
AC_SUBST(WITH_SUMMARIZATION)
AC_SUBST(WITH_DISTRIB)
AC_SUBST(WITH_CLUSTER)
AC_SUBST(HOST_OS)

dnl Base lemur modules
MODULES="retrieval langmod index parsing utility contrib/antlr contrib/xpdf"

AC_ARG_ENABLE(distrib,
	    AC_HELP_STRING([--enable-distrib],
	                   [Enable distributed retrieval package]))
if test "$enable_distrib" = "yes" ; then
  MODULES="distrib $MODULES"
  WITH_DISTRIB=1
  AC_DEFINE(WITH_DISTRIB, 1, [Enable distributed retrieval package])
else
  WITH_DISTRIB=0
  AC_DEFINE(WITH_DISTRIB, 0, [Enable distributed retrieval package])
fi

AC_ARG_ENABLE(summarization,
	    AC_HELP_STRING([--enable-summarization],
	                   [Enable summarization package]))

if test "$enable_summarization" = "yes" ; then
  MODULES="summarization $MODULES"
  WITH_SUMMARIZATION=1
  AC_DEFINE(WITH_SUMMARIZATION, 1, [Enable summarization package])
else
  WITH_SUMMARIZATION=0
  AC_DEFINE(WITH_SUMMARIZATION, 0, [Enable summarization package])
fi

AC_ARG_ENABLE(assert,
	    AC_HELP_STRING([--enable-assert],
	                   [Enable assert statements in code]),
			   [if test "${enable_assert}" = "no" ; then
			    AC_DEFINE(NDEBUG, 1, [Turn off assert statements])
			   fi],
			   AC_DEFINE(NDEBUG, 1, [Turn off assert statements]))

AC_ARG_ENABLE(cluster,
	    AC_HELP_STRING([--enable-cluster],
	                   [Enable cluster package]))

if test "$enable_cluster" = "yes" ; then
  MODULES="cluster $MODULES"
  WITH_CLUSTER=1
  AC_DEFINE(WITH_CLUSTER, 1, [Enable cluster package])
else
  WITH_CLUSTER=0
  AC_DEFINE(WITH_CLUSTER, 0, [Enable cluster package])
fi

AC_OUTPUT
