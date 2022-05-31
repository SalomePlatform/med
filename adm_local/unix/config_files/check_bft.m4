dnl Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

AC_DEFUN([CHECK_BFT],[

AC_CHECKING(for BFT Library)

AC_LANG_SAVE
AC_LANG(C++)

BFT_INCLUDES=""
BFT_LIBSUFFIX=""
BFT_LIBS=""

AC_CHECKING(for BFT location)
AC_ARG_WITH(bft,
            [  --with-bft=DIR      root directory path to BFT library installation ],
            [BFTDIR="$withval"
             AC_MSG_RESULT("select $withval as path to BFT library")])

AC_MSG_RESULT(\$BFTDIR = ${BFTDIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${BFTDIR}" != "x" ; then
  BFT_INCLUDES="-I${BFTDIR}/include"
  BFT_LIBS="-L${BFTDIR}/lib -lbft"
fi

bft_ok=no
bft_headers_ok=no
bft_binaries_ok=no

dnl BFT headers
AC_CHECKING(for BFT headers)
CPPFLAGS="${CPPFLAGS_old} ${BFT_INCLUDES} ${MPI_CPPFLAGS} ${MED_CPPFLAGS}"

bft_include_dir_ok=yes
if test "x${BFTDIR}" != "x" ; then
  AC_CHECK_FILE(${BFTDIR}/include/bft_mem.h,
                bft_include_dir_ok=yes,
                bft_include_dir_ok=no)
fi

if test "x${bft_include_dir_ok}" = "xyes" ; then
  AC_TRY_COMPILE([ #include "bft_mem.h"],
                 [bft_mem_init("")],
                 bft_headers_ok=yes,
                 bft_headers_ok=no)
fi

if test "x${bft_headers_ok}" = "xno" ; then
  BFT_INCLUDES="BFT_INCLUDES_NOT_DEFINED"
else
  AC_MSG_RESULT(\$BFT_INCLUDES = ${BFT_INCLUDES})
fi
AC_MSG_RESULT(for bft headers: $bft_headers_ok)

if test "x${bft_headers_ok}" = "xyes" ; then
  dnl BFT binaries
  AC_CHECKING(for BFT binaries)
  bft_lib_dir_ok=yes
  if test "x${BFTDIR}" != "x" ; then
    AC_CHECK_FILE(${BFTDIR}/lib/libbft.so,
                  bft_lib_dir_ok=yes,
                  bft_lib_dir_ok=no)
  fi

fi

CPPFLAGS="${CPPFLAGS_old}"
LIBS="${LIBS_old}"

if test "x${bft_headers_ok}" = "xyes" ; then
  if test "x${bft_lib_dir_ok}" = "xyes" ; then
    bft_ok=yes
  fi
fi

AC_MSG_RESULT(for bft: $bft_ok)

AC_SUBST(BFT_INCLUDES)
AC_SUBST(BFT_LIBSUFFIX)
AC_SUBST(BFT_LIBS)

AC_LANG_RESTORE

])dnl


