#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tinymtpeke], [0.1], [saito@manieth.com])
#LT_INIT
AC_CONFIG_SRCDIR([develop/Makefile.am])
AC_CONFIG_HEADERS([develop/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# Checks for programs.
: ${CFLAGS=""}
: ${CXXFLAGS=""}
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_LIBTOOL

AC_CHECK_LIB(gf2x, gf2x_mul)
AC_CHECK_LIB(gmp, __gmpz_init)
AC_CHECK_LIB(ntl, main,[],[ AC_MSG_ERROR(Need ntl) ])
#not work
#AX_CXX_CHECK_LIB(MTToolBox, [MTToolBox::get_mttoolbox_version],
#  [],[ AC_MSG_ERROR(Need MTToolBox) ])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h])
#AX_EXT

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_INT64_T
#AC_TYPE_UINT64_T

AC_LANG_PUSH(C++)
AX_CXX_COMPILE_STDCXX_0X

AX_CXX_COMPILE_STDCXX_14(noext, optional) // keep this order
AX_CXX_COMPILE_STDCXX_11(noext, optional)
AC_LANG_POP

AC_CONFIG_FILES([Makefile develop/Makefile])
AC_OUTPUT
