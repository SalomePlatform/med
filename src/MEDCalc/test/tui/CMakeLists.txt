# Copyright (C) 2016-2023  CEA, EDF
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

INCLUDE(tests.set)

SET(_test_files
    medfiles/smooth_surface_and_field.med
    medfiles/agitateur.med
    medfiles/deplacements.med
    medfiles/portico_elno.med
    medfiles/source_1.med
    medfiles/target_1.med
)

SET(TEST_INSTALL_DIRECTORY ${SALOME_FIELDS_INSTALL_TEST}/MEDCalc/tui)

FOREACH(tfile ${TEST_NAMES})
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py
          DESTINATION ${TEST_INSTALL_DIRECTORY})
ENDFOREACH()

INSTALL(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)

INSTALL(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

INSTALL(FILES ${_test_files} DESTINATION ${SALOME_FIELDS_INSTALL_RES_DATA}/tests/tui/medfiles)
