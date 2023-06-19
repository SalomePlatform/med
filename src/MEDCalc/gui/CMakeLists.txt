# Copyright (C) 2012-2023  CEA, EDF
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

IF(SALOME_FIELDS_WITH_QTTESTING)
  ADD_DEFINITIONS(-DMED_HAS_QTTESTING)
ENDIF()

ADD_DEFINITIONS(${MEDFILE_DEFINITIONS} ${XDR_DEFINITIONS} ${OMNIORB_DEFINITIONS})
SET(QT_LIBS "${QT_LIBRARIES}")
INCLUDE(UseQtExt)

ADD_SUBDIRECTORY(dialogs)

IF(WIN32)
 ADD_DEFINITIONS(-DNOMINMAX)
ENDIF(WIN32)

SET(MEDCALCGUI_SOURCES 
    DatasourceController.cxx
    factory.cxx
    MEDEventListener_i.cxx
    MEDModule.cxx
    PresentationController.cxx
    ProcessingController.cxx
    WorkspaceController.cxx
    XmedConsoleDriver.cxx
    XmedDataModel.cxx
    MEDWidgetHelper.cxx
    MEDWidgetHelperMeshView.cxx
    MEDWidgetHelperComponent.cxx
    MEDWidgetHelperContour.cxx
    MEDWidgetHelperSlices.cxx
    MEDWidgetHelperPlot3D.cxx
    MEDWidgetHelperStreamLines.cxx
    MEDWidgetHelperCutSegment.cxx
    MEDWidgetHelperVectorField.cxx
    MEDCALCGUI_Displayer.cxx
)

IF(SALOME_FIELDS_WITH_QTTESTING)
  LIST(APPEND MEDCALCGUI_SOURCES TestController.cxx)
ENDIF()

SET(MEDCALCGUI_MOC_HEADERS
  MEDModule.hxx
  MEDEventListener_i.hxx
  WorkspaceController.hxx
  DatasourceController.hxx
  PresentationController.hxx
  ProcessingController.hxx
  MEDWidgetHelper.hxx
  MEDWidgetHelperMeshView.hxx
  MEDWidgetHelperComponent.hxx
  MEDWidgetHelperScalarMap.hxx
  MEDWidgetHelperContour.hxx
  MEDWidgetHelperSlices.hxx
  MEDWidgetHelperPointSprite.hxx
  MEDWidgetHelperPlot3D.hxx
  MEDWidgetHelperStreamLines.hxx
  MEDWidgetHelperCutSegment.hxx
  MEDWidgetHelperVectorField.hxx
  MEDWidgetHelperDeflectionShape.hxx
  )

SET(MEDCALCGUI_HEADERS
  MEDCALCGUI_Displayer.hxx
  )
 
IF(SALOME_FIELDS_WITH_QTTESTING)
  LIST(APPEND MEDCALCGUI_MOC_HEADERS TestController.hxx)
ENDIF()

QT_WRAP_MOC(MEDCALCGUI_HEADERS_MOC ${MEDCALCGUI_MOC_HEADERS})

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR} ${OMNIORB_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${GUI_INCLUDE_DIRS}
  ${MEDCOUPLING_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
  ${CMAKE_CURRENT_BINARY_DIR}/dialogs
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../cmp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../MEDGUI
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
)

SET(MEDCALCGUITS_SOURCES
  FIELDS_images.ts
  FIELDS_msg_en.ts
  FIELDS_msg_fr.ts
  FIELDS_msg_ja.ts
  MEDCALC_msg_en.ts
  MEDCALC_msg_fr.ts
  MEDCALC_msg_ja.ts
)

SET(link_libraries
  ${QT_MT_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  SalomeIDLMED
  MEDCALCGUI_dialogs
  MEDFactoryEngine
  FIELDSEngine
  ${GUI_qtx}
  ${GUI_suit}
  ${GUI_SalomeObject}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_SalomeKernelHelpers}
  ${KERNEL_SalomeDS}
  ${GUI_SalomeApp}
  ${GUI_SalomeGuiHelpers}
  ${GUI_SalomeTreeData}
  ${KERNEL_OpUtil}
  ${GUI_CAM}
  ${GUI_LightApp}
  ${GUI_PyConsole}
  ${MEDCoupling_medloader}
  ${MEDCoupling_medcoupling}
  ${MEDCoupling_medcouplingremapper}
)

IF(SALOME_FIELDS_WITH_QTTESTING)
  LIST(APPEND link_libraries QtTesting)  # from ParaView
ENDIF()

ADD_LIBRARY(MEDCALCGUI SHARED ${MEDCALCGUI_SOURCES} ${MEDCALCGUI_HEADERS_MOC})
IF(WITH_MEDMEMGUI)
  SET_TARGET_PROPERTIES(MEDCALCGUI PROPERTIES COMPILE_FLAGS "-D__WITH_MEDMEMGUI__")
ENDIF(WITH_MEDMEMGUI)
TARGET_LINK_LIBRARIES(MEDCALCGUI ${link_libraries})

INSTALL(TARGETS MEDCALCGUI DESTINATION ${SALOME_INSTALL_LIBS})
QT_INSTALL_TS_RESOURCES("${MEDCALCGUITS_SOURCES}" "${SALOME_FIELDS_INSTALL_RES_DATA}")

FILE(GLOB COMMON_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
INSTALL(FILES ${COMMON_HEADERS_HXX} DESTINATION ${SALOME_INSTALL_HEADERS})
