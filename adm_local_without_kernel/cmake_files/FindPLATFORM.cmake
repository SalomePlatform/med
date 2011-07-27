#  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

MARK_AS_ADVANCED(ISSUE)
FIND_FILE(ISSUE issue /etc)
IF(ISSUE)
  SET(WINDOWS 0)
ELSE()
  SET(WINDOWS 1)
ENDIF(ISSUE)

IF(WINDOWS)
  SET(MACHINE WINDOWS)
ELSE(WINDOWS)
  SET(MACHINE PCLINUX)
ENDIF(WINDOWS)

SET(CMAKE_INSTALL_PREFIX_ENV $ENV{CMAKE_INSTALL_PREFIX})
IF(CMAKE_INSTALL_PREFIX_ENV)
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_ENV} CACHE PATH "installation prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_ENV)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(CMAKE_BUILD_TYPE)
  IF(WINDOWS)
    MARK_AS_ADVANCED(CLEAR CMAKE_CONFIGURATION_TYPES)
    SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "compilation types" FORCE)
  ELSE(WINDOWS)
    IF(CMAKE_BUILD_TYPE STREQUAL Release)
      SET(CMAKE_C_FLAGS_RELEASE "-O1 -DNDEBUG")
      SET(CMAKE_CXX_FLAGS_RELEASE "-O1 -DNDEBUG")
    ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
  ENDIF(WINDOWS)
ENDIF(CMAKE_BUILD_TYPE)

SET(PLATFORM_CPPFLAGS)
SET(PLATFORM_LDFLAGS)
SET(PLATFORM_LIBADD)
IF(WINDOWS)
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} /W0)
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -D_CRT_SECURE_NO_WARNINGS)  # To disable windows warnings for strcpy, fopen, ...
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -D_SCL_SECURE_NO_WARNINGS)  # To disable windows warnings std::copy, std::transform, ...
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -DWNT)
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -DPPRO_NT) # For medfile
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -D_USE_MATH_DEFINES) # At least for MEDMEM
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -D_WIN32_WINNT=0x0500) # Windows 2000 or later API is required
  SET(PLATFORM_LIBADD ${PLATFORM_LIBADD} Ws2_32.lib)
  SET(PLATFORM_LIBADD ${PLATFORM_LIBADD} Userenv.lib) # At least for GEOM suit
ELSE(WINDOWS)
  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -Wall)
  SET(PLATFORM_LIBADD ${PLATFORM_LIBADD} -ldl)
ENDIF(WINDOWS)

SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -DSIZEOF_FORTRAN_INTEGER=4 -DSIZEOF_LONG=${CMAKE_SIZEOF_VOID_P} -DSIZEOF_INT=4)

#  SET(PLATFORM_CPPFLAGS)
#  #  #  SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -DWNT -D_CRT_SECURE_NO_WARNINGS)
#  #  SET(RM del)
#ELSE(WINDOWS)
#  #  SET(RM /bin/rm)
#ENDIF(WINDOWS)

### SET(PLATFORM_CPPFLAGS ${PLATFORM_CPPFLAGS} -D_DEBUG_)

##SET(RCP rcp)
##SET(RSH rsh)
##SET(RCP rcp)

## MESSAGE(FATAL_ERROR "ZZZZZZZ")
