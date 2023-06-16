dnl Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

AC_DEFUN([CHECK_METIS],[

dnl metis could be already found by CHECK_PARMETIS procedure

if [[ "x${metis_ok}" != "xyes" ]] ; then

AC_CHECKING(for METIS Library)

AC_LANG_SAVE
AC_LANG_C

METIS_CPPFLAGS=""
METIS_LIBSUFFIX=""
METIS_LIBS=""
ENABLE_METIS="no"

AC_CHECKING(for METIS location)
AC_ARG_WITH(metis,
            [  --with-metis=DIR      root directory path to METIS library installation ],
            [METISDIR="$withval"
             AC_MSG_RESULT("select $withval as path to METIS library")])

AC_MSG_RESULT(\$METISDIR = ${METISDIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${METISDIR}" != "x" ; then
  METIS_CPPFLAGS="-DMED_ENABLE_METIS -I${METISDIR}/Lib"
  METIS_LIBS="-L${METISDIR} -lmetis"
fi

metis_ok=no
metis_headers_ok=no
metis_binaries_ok=no

dnl METIS headers
AC_CHECKING(for METIS headers)
CPPFLAGS="${CPPFLAGS_old} ${METIS_CPPFLAGS}"

metis_include_dir_ok=yes
if test "x${METISDIR}" != "x" ; then
  AC_CHECK_FILE(${METISDIR}/Lib/metis.h,
                metis_include_dir_ok=yes,
                metis_include_dir_ok=no)
fi

if test "x${metis_include_dir_ok}" = "xyes" ; then
  AC_TRY_COMPILE([#include <metis.h>],
                 [Change2CNumbering(0,0,0)],
                 metis_headers_ok=yes,
                 metis_headers_ok=no)
fi

if test "x${metis_headers_ok}" = "xno" ; then
  METIS_CPPFLAGS="METIS_CPPFLAGS_NOT_DEFINED"
else
  AC_MSG_RESULT(\$METIS_CPPFLAGS = ${METIS_CPPFLAGS})
fi
AC_MSG_RESULT(for metis headers: $metis_headers_ok)

if test "x${metis_headers_ok}" = "xyes" ; then
  dnl METIS binaries
  AC_CHECKING(for METIS binaries)
  metis_lib_dir_ok=yes
  if test "x${METISDIR}" != "x" ; then
    AC_CHECK_FILE(${METISDIR}/libmetis.a,
                  metis_lib_dir_ok=yes,
                  metis_lib_dir_ok=no)
    if test "x${metis_lib_dir_ok}" = "xno" ; then
      METIS_LIBSUFFIX=""
      AC_CHECK_FILE(${METISDIR}/libmetis.a,
                    metis_lib_dir_ok=yes,
                    metis_lib_dir_ok=no)
    fi
  fi
  if test "x${metis_lib_dir_ok}" = "xyes" ; then
    LIBS="${LIBS_old} ${METIS_LIBS}"
    AC_TRY_LINK([#include <metis.h>],
                [Change2CNumbering(0,0,0)],
                metis_binaries_ok=yes,
                metis_binaries_ok=no)
    if test "x${metis_binaries_ok}" = "xno" ; then
      METIS_LIBSUFFIX=""
      LIBS="${LIBS_old} ${METIS_LIBS} "
      AC_TRY_LINK([#include <metis.h>],
                  [Change2CNumbering(0,0,0)],
                  metis_binaries_ok=yes,
                  metis_binaries_ok=no)
    fi
  fi
fi

if test "x${metis_binaries_ok}" = "xno" ; then
  METIS_LIBS="METIS_LIBS_NOT_FOUND"
  METIS_LIBSUFFIX="-not-defined"
else
  AC_MSG_RESULT(\$METIS_LIBSUFFIX = ${METIS_LIBSUFFIX})
  AC_MSG_RESULT(\$METIS_LIBS = ${METIS_LIBS})
fi
AC_MSG_RESULT(for metis binaries: $metis_binaries_ok)

CPPFLAGS="${CPPFLAGS_old}"
LIBS="${LIBS_old}"

if test "x${metis_headers_ok}" = "xyes" ; then
  if test "x${metis_binaries_ok}" = "xyes" ; then
    metis_ok=yes
    ENABLE_METIS="yes"
  fi
fi

AC_MSG_RESULT(for metis: $metis_ok)

AC_SUBST(METIS_CPPFLAGS)
AC_SUBST(METIS_LIBSUFFIX)
AC_SUBST(METIS_LIBS)
AC_SUBST(ENABLE_METIS)

AC_LANG_RESTORE

fi

])dnl
