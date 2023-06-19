dnl Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

# Check availability of Fields binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_FIELDS],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl
AC_REQUIRE([CHECK_MED3])

AC_CHECKING(for Fields)

Fields_ok=no

FIELDS_LDFLAGS=""
FIELDS_CXXFLAGS=""

AC_ARG_WITH(fields,
	    [  --with-fields=DIR root directory path of Fields installation ],
	    FIELDS_DIR="$withval",FIELDS_DIR="")

if test "x${FIELDS_DIR}" == "x" ; then
   AC_MSG_RESULT(for \${FIELDS_ROOT_DIR}: ${FIELDS_ROOT_DIR})
   # --with-fields option is not used
   if test "x${FIELDS_ROOT_DIR}" != "x" ; then
      # FIELDS_ROOT_DIR environment variable defined
      FIELDS_DIR=${FIELDS_ROOT_DIR}
   fi
fi

if test -f ${FIELDS_DIR}/idl/salome/MEDCouplingCorbaServant.idl ; then
   AC_MSG_RESULT(Using Fields module distribution in ${FIELDS_DIR})
   Fields_ok=yes

   if test "x$FIELDS_ROOT_DIR" == "x" ; then
      FIELDS_ROOT_DIR=${FIELDS_DIR}
   fi

   AC_SUBST(FIELDS_ROOT_DIR)

   FIELDS_LDFLAGS=-L${FIELDS_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   FIELDS_CXXFLAGS=-I${FIELDS_DIR}/include/salome

   AC_SUBST(FIELDS_LDFLAGS)
   AC_SUBST(FIELDS_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find Fields module sources")
fi

AC_MSG_RESULT(for Fields: $Fields_ok)

dnl Backward compatibility
MED_ROOT_DIR=${FIELDS_ROOT_DIR}
MED_CXXFLAGS=${FIELDS_CXXFLAGS}
MED_LDFLAGS=${FIELDS_LDFLAGS}
AC_SUBST(MED_ROOT_DIR)
AC_SUBST(MED_CXXFLAGS)
AC_SUBST(MED_LDFLAGS)

])dnl

dnl Backward compatibility
AC_DEFUN([CHECK_MED],[CHECK_FIELDS])
