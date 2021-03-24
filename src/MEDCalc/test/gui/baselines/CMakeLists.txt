# Copyright (C) 2010-2021  CEA/DEN, EDF R&D
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

SET(TEST_NUMBERS 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)

SET(TEST_INSTALL_DIRECTORY ${SALOME_INSTALL_SCRIPT_SCRIPTS}/test/MEDReader)
SET(BASELINES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Baselines")

IF(KW_TESTING_PLATFORM)

cmake_minimum_required(VERSION 2.7)

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

include(ParaViewTestingMacros)
include(FindPythonModules)

include(${PROJECT_SOURCE_DIR}/testing/testFunctions.cmake)
set(PV_PLUGIN_PATH "${CMAKE_BINARY_DIR}/lib/")

include(CTest)

enable_testing()

set(PARAVIEW_BIN_DIR "${ParaView_DIR}/bin" CACHE PATH "ParaView binary path")

set(PVTEST_DRIVER "${PARAVIEW_BIN_DIR}/smTestDriver")
set(PARAVIEW_CLIENT "${PARAVIEW_BIN_DIR}/paraview")
set(PARAVIEW_SERVER "${PARAVIEW_BIN_DIR}/pvserver")
set(PVPYTHON "${PARAVIEW_BIN_DIR}/pvpython")

set(PYTHON_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(TEMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary")

if(NOT EXISTS ${TEMP_DIR})
  file(MAKE_DIRECTORY ${TEMP_DIR})
endif()

# Override vtk_add_test_* variables for use with ParaView.
macro (_myy_override_vtk_dirs)
  set(VTK_TEST_DATA_DIR    ${PYTHON_SCRIPTS_DIR})
  set(VTK_BASELINE_DIR     ${BASELINES_DIR})
  set(VTK_TEST_OUTPUT_DIR  ${TEMP_DIR})
  set(VTK_TEST_DATA_TARGET ParaViewData)
endmacro ()

function (myy_add_python_test)
  message(STATUS "Add Python test for ${ARGV2}")
  set(VTK_PYTHON_EXE "${PVPYTHON}")
  list(APPEND VTK_PYTHON_ARGS -dr
    ${PARAVIEW_PYTHON_ARGS})
  _myy_override_vtk_dirs()
  myvtk_add_test_python(${ARGN})
endfunction ()

# Test all python scripts
file(GLOB scripts "${PYTHON_SCRIPTS_DIR}/*py")
foreach(file ${scripts})
  get_filename_component(fname ${file} NAME)
  myy_add_python_test(
    NO_DATA NO_RT
    ${fname})
  INSTALL(FILES ${file}
          DESTINATION ${TEST_INSTALL_DIRECTORY})
endforeach()

ELSE(KW_TESTING_PLATFORM)

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

FOREACH(tfile ${TEST_NUMBERS})
  ADD_TEST(testMEDReader${tfile} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader${tfile}.py -B ${BASELINES_DIR} )
  SET_TESTS_PROPERTIES(testMEDReader${tfile} PROPERTIES ENVIRONMENT "${tests_env}")
ENDFOREACH()

ENDIF(KW_TESTING_PLATFORM)


# Application tests
FOREACH(tfile ${TEST_NUMBERS})
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/testMEDReader${tfile}.py
          DESTINATION ${TEST_INSTALL_DIRECTORY})
ENDFOREACH()

FILE(GLOB BASELINE_FILES "${BASELINES_DIR}/*")
FOREACH(baseline_file ${BASELINE_FILES})
  INSTALL(FILES ${baseline_file} DESTINATION ${TEST_INSTALL_DIRECTORY}/Baselines)
ENDFOREACH()

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${TEST_INSTALL_DIRECTORY}
        RENAME CTestTestfile.cmake)
