# Copyright (C) 2012-2022  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SALOME_CONFIGURE_FILE(medcalc_testutils.py.in ${CMAKE_CURRENT_BINARY_DIR}/medcalc_testutils.py)

IF(SALOME_BUILD_GUI)
  IF(SALOME_FIELDS_WITH_QTTESTING)
    ADD_SUBDIRECTORY(gui)
  ENDIF()
ENDIF()

IF(SALOME_FIELDS_ENABLE_PYTHON)
  ADD_SUBDIRECTORY(tui)
ENDIF()

SET(TEST_INSTALL_DIRECTORY ${SALOME_FIELDS_INSTALL_TEST}/MEDCalc)
SALOME_INSTALL_SCRIPTS(${CMAKE_CURRENT_BINARY_DIR}/medcalc_testutils.py ${SALOME_INSTALL_SCRIPT_SCRIPTS})
INSTALL(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
