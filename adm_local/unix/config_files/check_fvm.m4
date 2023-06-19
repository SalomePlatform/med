dnl Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

AC_DEFUN([CHECK_FVM],[

AC_CHECKING(for FVM Library)

AC_LANG_SAVE
AC_LANG(C++)

FVM_INCLUDES=""
FVM_LIBSUFFIX=""
FVM_LIBS=""
ENABLE_FVM="no"

AC_CHECKING(for FVM location)
AC_ARG_WITH(fvm,
            [  --with-fvm=DIR      root directory path to FVM library installation ],
            [FVMDIR="$withval"
             AC_MSG_RESULT("select $withval as path to FVM library")])

AC_MSG_RESULT(\$FVMDIR = ${FVMDIR})

CPPFLAGS_old="${CPPFLAGS}"
LIBS_old=$LIBS

if test "x${FVMDIR}" != "x" ; then
  
  CHECK_BFT
  fvm_ok=no



  if test "x${bft_ok}" = "xyes"; then
      fvm_headers_ok=no
      fvm_binaries_ok=no
      FVM_INCLUDES="-I${FVMDIR}/include"
      FVM_LIBS="-L${FVMDIR}/lib -lfvm ${BFT_LIBS} ${MPI_LIBS}"


    dnl FVM headers
      AC_CHECKING(for FVM headers)
      CPPFLAGS="${CPPFLAGS_old} ${FVM_INCLUDES} ${MPI_CPPFLAGS} ${MED_CPPFLAGS}"

      fvm_include_dir_ok=yes
      if test "x${FVMDIR}" != "x" ; then
	AC_CHECK_FILE(${FVMDIR}/include/fvm_nodal.h,
		      fvm_include_dir_ok=yes,
		      fvm_include_dir_ok=no)
      fi

      if test "x${fvm_include_dir_ok}" = "xyes" ; then
	AC_TRY_COMPILE([#include <iostream>
		      #include "fvm_nodal.h"],
		       [fvm_nodal_create("",2)],
		       fvm_headers_ok=yes,
		       fvm_headers_ok=no)
      fi

      if test "x${fvm_headers_ok}" = "xno" ; then
	FVM_INCLUDES="FVM_INCLUDES_NOT_DEFINED"
      else
	AC_MSG_RESULT(\$FVM_INCLUDES = ${FVM_INCLUDES})
      fi
      AC_MSG_RESULT(for fvm headers: $fvm_headers_ok)

      if test "x${fvm_headers_ok}" = "xyes" ; then
	dnl FVM binaries
	AC_CHECKING(for FVM binaries)
	fvm_lib_dir_ok=yes
	if test "x${FVMDIR}" != "x" ; then
	  AC_CHECK_FILE(${FVMDIR}/lib/libfvm.so,
			fvm_lib_dir_ok=yes,
			fvm_lib_dir_ok=no)
	fi

      fi

      CPPFLAGS="${CPPFLAGS_old}"
      LIBS="${LIBS_old}"

      if test "x${fvm_headers_ok}" = "xyes" ; then
	if test "x${fvm_lib_dir_ok}" = "xyes" ; then
	  fvm_ok=yes
	  ENABLE_FVM="yes"
	fi
      fi

     
      AC_SUBST(FVM_INCLUDES)
      AC_SUBST(FVM_LIBSUFFIX)
      AC_SUBST(FVM_LIBS)
			AC_SUBST(ENABLE_FVM)
  fi
  AC_MSG_RESULT(for fvm: $fvm_ok)

fi

AC_LANG_RESTORE

])dnl


