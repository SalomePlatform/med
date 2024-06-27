# Copyright (C) 2012-2024  CEA, EDF
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)

# Project name
# ============
# original
PROJECT(SalomeFIELDS C CXX)
# upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

# Common CMake macros
# ===================
SET(CONFIGURATION_ROOT_DIR $ENV{CONFIGURATION_ROOT_DIR} CACHE PATH "Path to the Salome CMake configuration files")
IF(EXISTS ${CONFIGURATION_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${CONFIGURATION_ROOT_DIR}/cmake")
  INCLUDE(SalomeMacros NO_POLICY_SCOPE)
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need the Salome CMake configuration files, please define CONFIGURATION_ROOT_DIR !")
ENDIF()

# Versioning
# ===========
SALOME_SETUP_VERSION(9.13.0)
MESSAGE(STATUS "Building ${PROJECT_NAME_UC} ${${PROJECT_NAME_UC}_VERSION} from \"${${PROJECT_NAME_UC}_GIT_SHA1}\"")

#
# Kernel detection comes before user options to be
# able to take previous values of global options ...
#
SET(_default_MPI OFF)
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  KERNEL_WITH_CORBA() #check whether KERNEL built with CORBA
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
  SET(_default_MPI ${SALOME_USE_MPI})
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR !")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)

FIND_PACKAGE(SalomePThread REQUIRED) # for MEDCouplingCorba/Test for EDF
FIND_PACKAGE(SalomeOmniORB REQUIRED)
#
# User options
# ============
INCLUDE(CMakeDependentOption)
OPTION(SALOME_FIELDS_ENABLE_PYTHON "Build PYTHON bindings." ON)
OPTION(SALOME_FIELDS_WITH_FILE_EXAMPLES "Install examples of files containing meshes and fields of different formats." ON)
OPTION(SALOME_FIELDS_WITH_QTTESTING "Build FIELDS with QtTesting support." OFF)
OPTION(SALOME_USE_MPI "(Use MPI containers) - For FIELDS this triggers the build of ParaMEDMEM." ${_default_MPI})
OPTION(SALOME_BUILD_GUI "Build GUI of FIELDS." ON)
OPTION(SALOME_BUILD_TESTS "Build FIELDS tests." ON)
OPTION(SALOME_BUILD_DOC "Build FIELDS doc." ON)

# Find MEDCoupling (required)
# ==========================
SET(MEDCOUPLING_ROOT_DIR $ENV{MEDCOUPLING_ROOT_DIR} CACHE PATH "Path to the MEDCoupling tool")
IF(EXISTS ${MEDCOUPLING_ROOT_DIR})
  FIND_PACKAGE(SalomeMEDCoupling REQUIRED)   # will reload HDF5, MEDFile, XDR, etc ...
ELSE(EXISTS ${MEDCOUPLING_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need the MEDCoupling tool, please define MEDCOUPLING_ROOT_DIR !")
ENDIF(EXISTS ${MEDCOUPLING_ROOT_DIR})

#
# Set list of prerequisites
# =========================
# XDR stuff
IF(NOT WIN32)
  FIND_PACKAGE(SalomeXDR REQUIRED)
ENDIF(NOT WIN32)
# End of XDR Stuff

# Find GUI (optional)
# ===========
IF(SALOME_BUILD_GUI)
    SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "Path to the Salome GUI")
    IF(EXISTS ${GUI_ROOT_DIR})
      FIND_PACKAGE(SalomeGUI REQUIRED)
      SALOME_GUI_WITH_CORBA() #check whether GUI built with CORBA
      SALOME_GUI_MODE(SALOME_USE_SALOMEOBJECT SALOME_USE_PYCONSOLE OPTIONAL SALOME_USE_PVVIEWER)
      ADD_DEFINITIONS(${GUI_DEFINITIONS})
      INCLUDE_DIRECTORIES(${GUI_INCLUDE_DIRS})
    ELSE(EXISTS ${GUI_ROOT_DIR})
      MESSAGE(FATAL_ERROR "We absolutely need a Salome GUI, please define GUI_ROOT_DIR or turn option SALOME_BUILD_GUI to OFF !")
    ENDIF(EXISTS ${GUI_ROOT_DIR})
    FIND_PACKAGE(SalomeQt5 REQUIRED)
ENDIF(SALOME_BUILD_GUI)

IF(SALOME_FIELDS_ENABLE_PYTHON)
  FIND_PACKAGE(SalomePythonInterp)
  FIND_PACKAGE(SalomePythonLibs)
  FIND_PACKAGE(SalomeNumPySciPy)
  FIND_PACKAGE(SalomeSWIG)
  SALOME_LOG_OPTIONAL_PACKAGE(PythonInterp SALOME_FIELDS_ENABLE_PYTHON)
  SALOME_LOG_OPTIONAL_PACKAGE(PythonLibs SALOME_FIELDS_ENABLE_PYTHON)
  SALOME_LOG_OPTIONAL_PACKAGE(SWIG   SALOME_FIELDS_ENABLE_PYTHON)
  FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
ENDIF(SALOME_FIELDS_ENABLE_PYTHON)

IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
  FIND_PACKAGE(SalomeCppUnit)
  SALOME_LOG_OPTIONAL_PACKAGE(CppUnit SALOME_BUILD_TESTS)
ENDIF(SALOME_BUILD_TESTS)

IF(SALOME_USE_MPI)
  FIND_PACKAGE(SalomeMPI REQUIRED)
  ADD_DEFINITIONS("-DHAVE_MPI")
ENDIF(SALOME_USE_MPI)

IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen)
  FIND_PACKAGE(SalomeGraphviz)
  FIND_PACKAGE(SalomeSphinx)
  SALOME_LOG_OPTIONAL_PACKAGE(Doxygen SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Graphviz SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Sphinx SALOME_BUILD_DOC)
ENDIF(SALOME_BUILD_DOC)

# Detection report
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).
# ===========
SET(SALOME_INSTALL_BINS bin/salome CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS lib/salome CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_IDLS idl/salome CACHE PATH "Install path: SALOME IDL files")
SET(SALOME_INSTALL_HEADERS include/salome CACHE PATH "Install path: SALOME headers")
SET(SALOME_INSTALL_SCRIPT_SCRIPTS ${SALOME_INSTALL_BINS} CACHE PATH
   "Install path: SALOME scripts")
SET(SALOME_INSTALL_SCRIPT_DATA ${SALOME_INSTALL_BINS} CACHE PATH
   "Install path: SALOME script data")
SET(SALOME_INSTALL_SCRIPT_PYTHON ${SALOME_INSTALL_BINS} CACHE PATH
   "Install path: SALOME Python scripts")
SET(SALOME_INSTALL_APPLISKEL_SCRIPTS ${SALOME_INSTALL_BINS}/appliskel CACHE PATH
   "Install path: SALOME application skeleton - scripts")
SET(SALOME_INSTALL_APPLISKEL_PYTHON ${SALOME_INSTALL_BINS}/appliskel CACHE PATH
   "Install path: SALOME application skeleton - Python")
SET(SALOME_INSTALL_CMAKE salome_adm/cmake_files CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL adm_local/cmake_files CACHE PATH
   "Install path: local SALOME CMake files")

IF(SALOME_FIELDS_ENABLE_PYTHON)
  SET(_pydir lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
  SET(SALOME_INSTALL_PYTHON ${_pydir}/salome CACHE PATH "Install path: SALOME Python stuff")
  SET(SALOME_INSTALL_PYTHON_SHARED ${SALOME_INSTALL_PYTHON}/shared_modules CACHE PATH
    "Install path: SALOME Python shared modules")
ENDIF(SALOME_FIELDS_ENABLE_PYTHON)

SET(SALOME_INSTALL_RES share/salome/resources CACHE PATH "Install path: SALOME resources")
SET(SALOME_INSTALL_DOC share/doc/salome CACHE PATH "Install path: SALOME documentation")

# Med specific:

SET(SALOME_FIELDS_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/fields" CACHE PATH "Install path: SALOME MED specific data")
SET(SALOME_FIELDS_INSTALL_RES_SCRIPTS "${SALOME_INSTALL_RES}/fields" CACHE PATH "Install path: SALOME MED specific scripts")
SET(SALOME_FIELDS_INSTALL_TEST ${SALOME_INSTALL_BINS}/test)

MARK_AS_ADVANCED(SALOME_INSTALL_BINS SALOME_INSTALL_LIBS SALOME_INSTALL_IDLS SALOME_INSTALL_HEADERS)
MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_SCRIPTS SALOME_INSTALL_SCRIPT_DATA SALOME_INSTALL_SCRIPT_PYTHON)
MARK_AS_ADVANCED(SALOME_INSTALL_APPLISKEL_SCRIPTS  SALOME_INSTALL_APPLISKEL_PYTHON SALOME_INSTALL_CMAKE SALOME_INSTALL_CMAKE_LOCAL SALOME_INSTALL_RES)
MARK_AS_ADVANCED(SALOME_INSTALL_PYTHON SALOME_INSTALL_PYTHON_SHARED SALOME_FIELDS_INSTALL_RES_DATA SALOME_FIELDS_INSTALL_RES_SCRIPTS SALOME_INSTALL_DOC)

# Header configuration
# ====================
SALOME_CONFIGURE_FILE(FIELDS_version.h.in FIELDS_version.h INSTALL ${SALOME_INSTALL_HEADERS})
SALOME_CONFIGURE_FILE(VERSION.in VERSION INSTALL ${SALOME_INSTALL_BINS})

# Accumulate environment variables for MED module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS})


# Sources
# ========
IF(WIN32)
  ADD_DEFINITIONS("-D_USE_MATH_DEFINES")
ENDIF(WIN32)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(adm_local)

IF(SALOME_BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF(SALOME_BUILD_DOC)

ADD_SUBDIRECTORY(idl)
ADD_SUBDIRECTORY(resources)

# Application tests
# =================
INSTALL(FILES CTestTestfileInstall.cmake DESTINATION ${SALOME_FIELDS_INSTALL_TEST} RENAME CTestTestfile.cmake)

# Configuration export
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
SET(_${PROJECT_NAME}_exposed_targets SalomeIDLMED SalomeIDLMEDTests)

IF(SALOME_USE_MPI)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets paramedcouplingcorba paramedmemcompo)
ENDIF()

LIST(APPEND _${PROJECT_NAME}_exposed_targets
    medcouplingcorba medcouplingclient medcalculator MEDEngineCommon MEDFactoryEngine FIELDSEngine)
IF(SALOME_FIELDS_ENABLE_PYTHON)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets medcalculatorspython)
ENDIF()

IF(SALOME_BUILD_GUI)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets MEDCALCGUI_dialogs MEDCALCGUI)
ENDIF()

# Add all targets to the build-tree export set

EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure (even if empty):
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(GUI_ROOT_DIR "${GUI_ROOT_DIR}")
SET(MEDCOUPLING_ROOT_DIR "${MEDCOUPLING_ROOT_DIR}")
SET(MPI_ROOT_DIR "${MPI_ROOT_DIR}")
SET(OMNIORB_ROOT_DIR "${OMNIORB_ROOT_DIR}")
SET(PTHREAD_ROOT_DIR "${PTHREAD_ROOT_DIR}")
SET(SWIG_ROOT_DIR "${SWIG_ROOT_DIR}")
SET(PTHREAD_ROOT_DIR "${SWIG_ROOT_DIR}")
SET(PYTHON_ROOT_DIR "${PYTHON_ROOT_DIR}")
SET(CPPUNIT_ROOT_DIR "${CPPUNIT_ROOT_DIR}")
SET(GRAPHVIZ_ROOT_DIR "${GRAPHVIZ_ROOT_DIR}")
SET(DOXYGEN_ROOT_DIR "${DOXYGEN_ROOT_DIR}")
SET(SPHINX_ROOT_DIR "${SPHINX_ROOT_DIR}")
SET(QT_ROOT_DIR "${QT5_ROOT_DIR}")

#   - in the install tree:
#       Get the relative path of the include directory so
#       we can register it in the generated configuration files:
SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
SALOME_CONFIGURE_PREPARE(MEDCoupling Qt5 MPI omniORB CppUnit Graphviz Doxygen Sphinx
                                                                        PThread Python)

CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
       KERNEL_ROOT_DIR GUI_ROOT_DIR MEDCOUPLING_ROOT_DIR QT_ROOT_DIR MPI_ROOT_DIR
       OMNIORB_ROOT_DIR PTHREAD_ROOT_DIR SWIG_ROOT_DIR PYTHON_ROOT_DIR CPPUNIT_ROOT_DIR
       GRAPHVIZ_ROOT_DIR DOXYGEN_ROOT_DIR SPHINX_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)

# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
        FILE ${PROJECT_NAME}Targets.cmake)

# Application tests
INSTALL(FILES CTestTestfileInstall.cmake
  DESTINATION ${SALOME_INSTALL_SCRIPT_SCRIPTS}/test
  RENAME CTestTestfile.cmake)
