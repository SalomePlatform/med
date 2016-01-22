# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

ADD_DEFINITIONS(${HDF5_DEFINITIONS} ${MEDFILE_DEFINITIONS} ${OMNIORB_DEFINITIONS})

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR} ${OMNIORB_INCLUDE_DIRS}
  ${MEDFILE_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${MEDCOUPLING_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
)

SET(COMMON_SOURCES
  MEDCommandsHistoryManager_i.cxx
  MEDFactoryClient.cxx
  MEDDataManager_i.cxx
  MEDCalculator_i.cxx
  MEDPresentationManager_i.cxx
  MEDPresentation.cxx
  # presentations
  MEDPresentationScalarMap.cxx
  MEDPresentationContour.cxx
  MEDPresentationVectorField.cxx
  MEDPresentationSlices.cxx
  MEDPresentationDeflectionShape.cxx
  MEDPresentationPointSprite.cxx
)

SET(MEDFactoryEngine_SOURCES
  ${COMMON_SOURCES}
  MEDFactory_i.cxx
)

SET(MEDEngine_SOURCES
  ${COMMON_SOURCES}
  MED.cxx
)

SET(COMMON_LIBS
  ${PLATFORM_LIBRARIES}
  SalomeIDLMED
  ${KERNEL_TOOLSDS} ${KERNEL_SalomeDS} ${KERNEL_SalomeHDFPersist} ${KERNEL_SalomeContainer} ${KERNEL_SalomeCommunication}
  ${KERNEL_SalomeKernelHelpers} ${KERNEL_SalomeLifeCycleCORBA} ${KERNEL_SALOMELocalTrace} ${KERNEL_SALOMEBasics}
  ${KERNEL_SalomeGenericObj}
  ${MEDFILE_C_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${MEDCoupling_medcoupling} ${MEDCoupling_medloader} ${MEDCoupling_medcouplingremapper}
)

# This undefines the macros MIN and MAX which are specified in the windows headers
IF(WIN32)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DNOMINMAX")
ENDIF(WIN32)

ADD_LIBRARY(MEDFactoryEngine SHARED ${MEDFactoryEngine_SOURCES})
SET_TARGET_PROPERTIES(MEDFactoryEngine PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(MEDFactoryEngine ${COMMON_LIBS})

ADD_LIBRARY(MEDEngine SHARED ${MEDEngine_SOURCES})
SET_TARGET_PROPERTIES(MEDEngine PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(MEDEngine ${COMMON_LIBS})

#INSTALL(TARGETS MEDFactoryEngine DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(TARGETS MEDFactoryEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(TARGETS MEDEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

FILE(GLOB MEDCALC_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
INSTALL(FILES ${MEDCALC_HEADERS_HXX} DESTINATION ${SALOME_INSTALL_HEADERS})
FILE(GLOB MEDCALC_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
INSTALL(FILES ${MEDCALC_HEADERS_HXX} DESTINATION ${SALOME_INSTALL_HEADERS})

IF(SALOME_ENABLE_PYTHON)
  INSTALL(FILES test_medcalc_components.py PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ DESTINATION ${SALOME_INSTALL_BINS}/xmed)
ENDIF()