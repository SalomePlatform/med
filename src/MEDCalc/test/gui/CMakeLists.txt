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

SET(_test_scripts
    test_scalarmap.py
)

SET(_test_scenarii
   scenarios/test_scalarmap.xml
)

SET(_test_baselines
    baselines/test_scalarmap.png
)

SET(_test_files
    medfiles/test_scalarmap.med
)

# Install rules
SET(TEST_INSTALL_DIRECTORY ${SALOME_FIELDS_INSTALL_TEST}/MEDCalc/gui)

SALOME_INSTALL_SCRIPTS(test_qttesting.py ${TEST_INSTALL_DIRECTORY})

SALOME_INSTALL_SCRIPTS("${_test_scripts}" ${SALOME_INSTALL_PYTHON}/tests/gui)

INSTALL(FILES ${_test_scenarii} DESTINATION ${SALOME_FIELDS_INSTALL_RES_DATA}/tests/gui/scenarios)
INSTALL(FILES ${_test_baselines} DESTINATION ${SALOME_FIELDS_INSTALL_RES_DATA}/tests/gui/baselines)
INSTALL(FILES ${_test_files} DESTINATION ${SALOME_FIELDS_INSTALL_RES_DATA}/tests/gui/medfiles)

INSTALL(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
