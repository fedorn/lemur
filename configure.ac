dnl Process this file with autoconf to produce a configure script.
AC_INIT([Lemur],[4.2],[project@lemurproject.org])
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
AC_SUBST(SHARED)
case $host in
   *-*-solaris*) 
     AC_CHECK_LIB(nsl, gethostbyname)
     AC_CHECK_LIB(socket, socket)
     SHARED="-shared -fPIC"
     ;;
   *linux*) 
     SHARED="-shared -fPIC"
     ;;
   *-*-darwin*)
     SHARED=
     ;;
   *);;
esac
AC_CHECK_LIB(z, deflate)
AC_CHECK_LIB(iberty, mkstemps)

dnl Checks for header files.
AC_DEFUN([AC_CXX_NAMESPACES],
	 [AC_MSG_CHECKING([whether C++ namespaces are supported])
	  AC_LANG_PUSH([C++])
	  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[namespace Outer { namespace Inner { int i = 0; }}]], [[using namespace Outer::Inner; return i;]])],[ac_cv_cxx_namespaces=yes],[ac_cv_cxx_namespaces=no])
          AC_LANG_POP
	  AC_MSG_RESULT($ac_cv_cxx_namespaces)
	  if test "$ac_cv_cxx_namespaces" = yes; then
		AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
          fi])
			      
AC_DEFUN([AC_CXX_ISNAN_IN_NAMESPACE_STD],
	[AC_REQUIRE([AC_CXX_NAMESPACES])
	AC_MSG_CHECKING([whether isnan is in namespace std])
	 AC_LANG_PUSH([C++])
	 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <cmath>
	   namespace lemur { int isnan(float x){ return std::isnan(x); } };]], [[
	   using namespace lemur; float x = 1.0; lemur::isnan(x); return 0;]])],[ac_cv_cxx_isnan_std=yes],[ac_cv_cxx_isnan_std=no])
         AC_LANG_POP
 	  AC_MSG_RESULT($ac_cv_cxx_isnan_std)
         if test "$ac_cv_cxx_isnan_std" = yes; then
	    AC_DEFINE(ISNAN_IN_NAMESPACE_STD,,[define if the compiler has isnan function in namespace std])
	 fi])

AC_CXX_ISNAN_IN_NAMESPACE_STD	       
dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_BIGENDIAN
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

dnl Checks for library functions.

AC_CHECK_FUNCS(mkstemp)
AC_CHECK_FUNCS(mkstemps)

dnl ########################################################################
    dnl see if using GNU glibc++, because then we have atomic integer operations

    P_HAS_ATOMIC_INT=0
    P_NEEDS_GNU_CXX_NAMESPACE=0
    AC_MSG_CHECKING(if atomic integer available)
    AC_LANG_PUSH([C++])

    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <bits/atomicity.h>]], [[_Atomic_word val;]])],[P_HAS_ATOMIC_INT=1],[])
		   if test ${P_HAS_ATOMIC_INT} = 1 ; then
		     AC_MSG_RESULT(yes)
		     AC_MSG_CHECKING(if __atomic_add is in __gnu_cxx namespace)
		     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <bits/atomicity.h>]], [[_Atomic_word val; __gnu_cxx::__atomic_add(&val, 1)]])],[P_NEEDS_GNU_CXX_NAMESPACE=1],[])
		     if test ${P_NEEDS_GNU_CXX_NAMESPACE} = 1 ; then
			AC_MSG_RESULT(yes)
		     else
			AC_MSG_RESULT(no)
		     fi
		   else
		     AC_MSG_RESULT(no)
		   fi
		   AC_LANG_POP
		   AC_DEFINE_UNQUOTED(P_HAS_ATOMIC_INT, ${P_HAS_ATOMIC_INT})
		   AC_DEFINE_UNQUOTED(P_NEEDS_GNU_CXX_NAMESPACE, ${P_NEEDS_GNU_CXX_NAMESPACE})
							  	
dnl Finish up and output.
AC_SUBST(MODULES)
AC_SUBST(WITH_SUMMARIZATION)
AC_SUBST(WITH_DISTRIB)
AC_SUBST(WITH_CLUSTER)
AC_SUBST(HOST_OS)

dnl Base lemur modules
MODULES="retrieval langmod index parsing utility contrib/antlr contrib/xpdf"

AC_ARG_ENABLE(distrib,
	    AS_HELP_STRING(--enable-distrib,Enable distributed retrieval package))
if test "$enable_distrib" = "yes" ; then
  MODULES="distrib $MODULES"
  WITH_DISTRIB=1
  AC_DEFINE(WITH_DISTRIB, 1, [Enable distributed retrieval package])
else
  WITH_DISTRIB=0
  AC_DEFINE(WITH_DISTRIB, 0, [Enable distributed retrieval package])
fi

AC_ARG_ENABLE(summarization,
	    AS_HELP_STRING(--enable-summarization,Enable summarization package))

if test "$enable_summarization" = "yes" ; then
  MODULES="summarization $MODULES"
  WITH_SUMMARIZATION=1
  AC_DEFINE(WITH_SUMMARIZATION, 1, [Enable summarization package])
else
  WITH_SUMMARIZATION=0
  AC_DEFINE(WITH_SUMMARIZATION, 0, [Enable summarization package])
fi

AC_ARG_ENABLE(assert,
	    AS_HELP_STRING(--enable-assert,Enable assert statements in code),
			   [if test "${enable_assert}" = "no" ; then
			    AC_DEFINE(NDEBUG, 1, [Turn off assert statements])
			   fi],
			   AC_DEFINE(NDEBUG, 1, [Turn off assert statements]))

AC_ARG_ENABLE(cluster,
	    AS_HELP_STRING(--enable-cluster,Enable cluster package))

if test "$enable_cluster" = "yes" ; then
  MODULES="cluster $MODULES"
  WITH_CLUSTER=1
  AC_DEFINE(WITH_CLUSTER, 1, [Enable cluster package])
else
  WITH_CLUSTER=0
  AC_DEFINE(WITH_CLUSTER, 0, [Enable cluster package])
fi

AC_OUTPUT
