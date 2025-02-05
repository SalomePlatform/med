dnl Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

AC_DEFUN([MED_CHECK_SIZEOF_MEDINT], [

  ## Guess the fortran int size
  AC_CHECK_SIZEOF_FORTRAN(integer)
  if test "x$ac_cv_sizeof_fortran_integer" = "x8" ; then
     AC_CHECK_SIZEOF(long)
     test "x$ac_cv_sizeof_long" = "x8" || AC_MSG_ERROR([Size of C type long expected to be eight bytes])
     DEFINED_F77INT64="-DHAVE_F77INT64"
  elif test "x$ac_cv_sizeof_fortran_integer" = "x4" ; then
     AC_CHECK_SIZEOF(int)
     test "x$ac_cv_sizeof_int" = "x4" || AC_MSG_ERROR([Size of C type int expected to be four bytes])
     DEFINED_F77INT64=""
  else
     AC_MSG_ERROR([Size of Fortran type integer is neither four nor eight bytes])
  fi
  AC_SUBST(DEFINED_F77INT64)
])