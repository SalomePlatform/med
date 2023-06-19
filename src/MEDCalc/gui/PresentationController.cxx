// Copyright (C) 2016-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PresentationController.hxx"
#include <MEDCalcConstants.hxx>
#include "MEDModule.hxx"
#include "Basics_Utils.hxx"
#include "QtxActionGroup.h"
#include "QtxActionToolMgr.h"
#include "MEDFactoryClient.hxx"
#include "MEDPresentationManager_i.hxx"
#include "XmedConsoleDriver.hxx"

#include "MEDPresentationMeshView.hxx"
#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationContour.hxx"
#include "MEDPresentationSlices.hxx"
#include "MEDPresentationPointSprite.hxx"
#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationPlot3D.hxx"
#include "MEDPresentationStreamLines.hxx"
#include "MEDPresentationCutSegment.hxx"
#include "MEDPresentationDeflectionShape.hxx"

#include "MEDWidgetHelperMeshView.hxx"
#include "MEDWidgetHelperScalarMap.hxx"
#include "MEDWidgetHelperContour.hxx"
#include "MEDWidgetHelperSlices.hxx"
#include "MEDWidgetHelperPointSprite.hxx"
#include "MEDWidgetHelperPlot3D.hxx"
#include "MEDWidgetHelperStreamLines.hxx"
#include "MEDWidgetHelperCutSegment.hxx"
#include "MEDWidgetHelperVectorField.hxx"
#include "MEDWidgetHelperDeflectionShape.hxx"

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataObject.h>

#include <SALOME_ListIO.hxx>
#include <LightApp_SelectionMgr.h>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <QMessageBox>
#include <sstream>
#include <regex>

#include "MEDFactoryClient.hxx"

static const int OPTIONS_VIEW_MODE_ID = 943;
static const int OPTIONS_VIEW_MODE_REPLACE_ID = 944;
static const int OPTIONS_VIEW_MODE_OVERLAP_ID = 945;
static const int OPTIONS_VIEW_MODE_NEW_LAYOUT_ID = 946;
static const int OPTIONS_VIEW_MODE_SPLIT_VIEW_ID = 947;

//! The only instance of the MEDPresentationManager
MEDCALC::MEDPresentationManager_ptr PresentationController::_presManager;

PresentationController::PresentationController(MEDModule* salomeModule) :
        _salomeModule(salomeModule),
        _consoleDriver(0),
        _studyEditor(salomeModule->getStudyEditor()),
        _presHelperMap(),
        _currentWidgetHelper(0)
{
  STDLOG("Creating a PresentationController");

  _widgetPresentationParameters = new WidgetPresentationParameters();

  QMainWindow* parent = salomeModule->getApp()->desktop();
  _dockWidget = new QDockWidget(parent);
  _dockWidget->setVisible(false);
  _dockWidget->setWindowTitle(tr("TITLE_PRESENTATION_PARAMETERS"));
  _dockWidget->setObjectName(tr("TITLE_PRESENTATION_PARAMETERS"));
  _dockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
  _dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _dockWidget->setWidget(_widgetPresentationParameters);
  parent->addDockWidget(Qt::LeftDockWidgetArea, _dockWidget);
  //_dockWidget->show();

  // Retrieve MEDFactory to get MEDPresentationManager (sometimes GUI needs to talk to the engine directly)
  if ( ! _presManager ) {
      _presManager = MEDFactoryClient::getFactory()->getPresentationManager();
  }

  // Connect to the click in the object browser
  connect(salomeModule, SIGNAL( presentationSelected(int , const QString&, const QString&) ),
          this, SLOT(onPresentationSelected(int , const QString&, const QString&) )     );
}

PresentationController::~PresentationController()
{
  STDLOG("Deleting the resentationController");
  // Clean allocated widget helpers:
  for ( std::map<int, MEDWidgetHelper *>::iterator it = _presHelperMap.begin(); it != _presHelperMap.end(); ++it)
    delete((*it).second);
}

std::string
PresentationController::_getIconName(const std::string& name)
{
  SUIT_ResourceMgr* mgr = SUIT_Session::session()->resourceMgr();
  if (!mgr)
    return name;

  // Read value from preferences and suffix name to select icon theme
  int theme = mgr->integerValue("MEDCalc", "icons");
  if (theme == 0) {
      return name + "_MODERN";
  } else if (theme == 1) {
      return name + "_CLASSIC";
  }
  return name + "_DEFAULT";
}

void
PresentationController::createActions()
{
  STDLOG("Creating PresentationController actions");

  // View Mode
  int viewModeToolbarId = _salomeModule->createTool("View Mode", "ViewModeToolbar");
  QtxActionGroup* ag = _salomeModule->createActionGroup(OPTIONS_VIEW_MODE_ID, true);
  ag->setText("View mode");
  ag->setUsesDropDown(true);
  QString label   = tr("LAB_VIEW_MODE_REPLACE");
  QString tooltip = tr("TIP_VIEW_MODE_REPLACE");
  QAction* a = _salomeModule->createAction(OPTIONS_VIEW_MODE_REPLACE_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  a->setChecked(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_OVERLAP");
  tooltip = tr("TIP_VIEW_MODE_OVERLAP");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_OVERLAP_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_NEW_LAYOUT");
  tooltip = tr("TIP_VIEW_MODE_NEW_LAYOUT");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_NEW_LAYOUT_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  label   = tr("LAB_VIEW_MODE_SPLIT_VIEW");
  tooltip = tr("TIP_VIEW_MODE_SPLIT_VIEW");
  a = _salomeModule->createAction(OPTIONS_VIEW_MODE_SPLIT_VIEW_ID,label,QIcon(),label,tooltip,0);
  a->setCheckable(true);
  ag->add(a);

  _salomeModule->createTool(OPTIONS_VIEW_MODE_ID, viewModeToolbarId);

  // Presentations
  int presentationToolbarId = _salomeModule->createTool("Presentations", "PresentationToolbar");
  int presentationMenuId = _salomeModule->createMenu(tr("MENU_PRESENTATIONS"), -1, -1, 10);

  label   = tr("LAB_PRESENTATION_MESH_VIEW");
  tooltip = tr("TIP_PRESENTATION_MESH_VIEW");
  QString icon = tr(_getIconName("ICO_PRESENTATION_MESH_VIEW").c_str());
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onVisualizeMeshView()),icon,tooltip);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_SCALAR_MAP");
  tooltip = tr("TIP_PRESENTATION_SCALAR_MAP");
  icon = tr(_getIconName("ICO_PRESENTATION_SCALAR_MAP").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeScalarMap()),
                                                 icon, tooltip, FIELDSOp::OpScalarMap);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_CONTOUR");
  tooltip = tr("TIP_PRESENTATION_CONTOUR");
  icon    = tr(_getIconName("ICO_PRESENTATION_CONTOUR").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeContour()),
                                                 icon, tooltip, FIELDSOp::OpContour);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_VECTOR_FIELD");
  tooltip = tr("TIP_PRESENTATION_VECTOR_FIELD");
  icon    = tr(_getIconName("ICO_PRESENTATION_VECTOR_FIELD").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeVectorField()),
                                                 icon, tooltip, FIELDSOp::OpVectorFields);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_SLICES");
  tooltip = tr("TIP_PRESENTATION_SLICES");
  icon    = tr(_getIconName("ICO_PRESENTATION_SLICES").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeSlices()),
                                                 icon, tooltip, FIELDSOp::OpSlices);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_DEFLECTION_SHAPE");
  tooltip = tr("TIP_PRESENTATION_DEFLECTION_SHAPE");
  icon    = tr(_getIconName("ICO_PRESENTATION_DEFLECTION_SHAPE").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeDeflectionShape()),
                                                 icon, tooltip, FIELDSOp::OpDeflectionShape);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_POINT_SPRITE");
  tooltip = tr("TIP_PRESENTATION_POINT_SPRITE");
  icon    = tr(_getIconName("ICO_PRESENTATION_POINT_SPRITE").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizePointSprite()),
                                                 icon, tooltip, FIELDSOp::OpPointSprite);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  // sphinx doc: begin of plot3d gui items
  label   = tr("LAB_PRESENTATION_PLOT3D");
  tooltip = tr("TIP_PRESENTATION_PLOT3D");
  icon    = tr(_getIconName("ICO_PRESENTATION_PLOT3D").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizePlot3D()),
                                                 icon, tooltip, FIELDSOp::OpPlot3D);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);
  // sphinx doc: end of plot3d gui items

  label   = tr("LAB_PRESENTATION_STREAM_LINES");
  tooltip = tr("TIP_PRESENTATION_STREAM_LINES");
  icon    = tr(_getIconName("ICO_PRESENTATION_STREAM_LINES").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeStreamLines()),
                                                 icon, tooltip, FIELDSOp::OpStreamLines);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  label   = tr("LAB_PRESENTATION_CUT_SEGMENT");
  tooltip = tr("TIP_PRESENTATION_CUT_SEGMENT");
  icon    = tr(_getIconName("ICO_PRESENTATION_CUT_SEGMENT").c_str());
  actionId = _salomeModule->createStandardAction(label, this, SLOT(onVisualizeCutSegment()),
                                                 icon, tooltip, FIELDSOp::OpCutSegment);
  _salomeModule->createTool(actionId, presentationToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);


  // Separator
  _salomeModule->createMenu(_salomeModule->separator(), presentationMenuId);

  label   = tr("LAB_DELETE_PRESENTATION");
  tooltip = tr("TIP_DELETE_PRESENTATION");
  icon    = tr("ICO_DELETE_PRESENTATION");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(onDeletePresentation()),icon,tooltip);
  //  _salomeModule->createTool(actionId, presentationToolbarId);
  //  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, presentationMenuId);

  // Low level PARAVIS dump
  label = tr("LAB_PARAVIS_DUMP");
  tooltip = tr("TIP_PARAVIS_DUMP");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(onParavisDump()),"");
  _salomeModule->createMenu(actionId, presentationMenuId);

  //
  // Actions for popup menu only
  //

}

MEDCALC::ViewModeType
PresentationController::getSelectedViewMode() const
{
  if (_salomeModule->action(OPTIONS_VIEW_MODE_REPLACE_ID)->isChecked()) {
      return MEDCALC::VIEW_MODE_REPLACE;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_OVERLAP_ID)->isChecked()) {
      return MEDCALC::VIEW_MODE_OVERLAP;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_NEW_LAYOUT_ID)->isChecked()) {
      return MEDCALC::VIEW_MODE_NEW_LAYOUT;
  }
  else if (_salomeModule->action(OPTIONS_VIEW_MODE_SPLIT_VIEW_ID)->isChecked()) {
      return MEDCALC::VIEW_MODE_SPLIT_VIEW;
  }
  // Should not happen
  STDLOG("Strange!! No matching view mode found - returning VIEW_MODE_REPLACE.");
  return MEDCALC::VIEW_MODE_REPLACE;
}

MEDCALC::ColorMapType
PresentationController::getSelectedColorMap() const
{
  return _widgetPresentationParameters->getColorMap();
}

MEDCALC::ScalarBarRangeType
PresentationController::getSelectedScalarBarRange() const
{
  return _widgetPresentationParameters->getScalarBarRange();
}

void
PresentationController::visualize(PresentationEvent::EventType eventType)
{
  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();

  // For each object, emit a signal to the workspace to request a
  // visualisation using the tui command (so that the user can see how
  // to make a view of an object from the tui console).
  for (int i=0; i<(int)listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soObj = listOfSObject->at(i);
      std::string name(_studyEditor->getName(soObj));
      if (soObj->_is_nil() || name == "MEDCalc")
        return;
      int fieldId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_ID);
      int meshId = _salomeModule->getIntParamFromStudyEditor(soObj, MESH_ID);
      MEDCALC::FieldHandler* fieldHandler = 0;
      MEDCALC::MeshHandler* meshHandler = 0;

      // is it a mesh?
      if (meshId >= 0)
        {
          if (eventType != PresentationEvent::EVENT_VIEW_OBJECT_MESH_VIEW)
            continue;
          meshHandler = MEDFactoryClient::getDataManager()->getMeshHandler(meshId);
        }
      else
        {
          if (fieldId < 0)  // is it a field series?
            {
              int fieldSeriesId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_SERIES_ID);
              // If fieldId and fieldSeriesId equals -1, then it means that it is not a field
              // managed by the MED module, and we stop this function process.
              if ( fieldSeriesId < 0)
                  continue;

              // get the current timestamp
              double timestamp = _salomeModule->getCurrentAnimationTimestamp();
              // get the field id a the current timestamp
              fieldId = MEDFactoryClient::getDataManager()->getFieldIdAtTimestamp(fieldSeriesId, timestamp);
            }
          fieldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
        }

      if ((!fieldHandler) && (!meshHandler)) {
          QMessageBox::warning(_salomeModule->getApp()->desktop(),
                               tr("Operation not allowed"),
                               tr("No field (or mesh) is defined"));
          return;
      }

      PresentationEvent* event = new PresentationEvent();
      event->eventtype = eventType;
      event->fieldHandler = fieldHandler;
      event->meshHandler = meshHandler;
      emit presentationSignal(event); // --> processPresentationEvent()
  }
}

void
PresentationController::onVisualizeMeshView()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_MESH_VIEW);
}

void
PresentationController::onVisualizeScalarMap()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_SCALAR_MAP);
}

void
PresentationController::onVisualizeContour()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_CONTOUR);
}

void
PresentationController::onVisualizeVectorField()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_VECTOR_FIELD);
}

void
PresentationController::onVisualizeSlices()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_SLICES);
}

void
PresentationController::onVisualizeDeflectionShape()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_DEFLECTION_SHAPE);
}

void
PresentationController::onVisualizePointSprite()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_POINT_SPRITE);
}

// sphinx doc: begin of onVisualizePlot3D
void
PresentationController::onVisualizePlot3D()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_PLOT3D);
}
// sphinx doc: end of onVisualizePlot3D

void
PresentationController::onVisualizeStreamLines()
{
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_STREAM_LINES);
}

void
PresentationController::onVisualizeCutSegment()
{
  // Cut segment presentation "creates" new view, so switch off visibility state update 
  // because pqActiveObjects::viewChanged is emmited 
  _salomeModule->visibilityStateUpdateOff();
  this->visualize(PresentationEvent::EVENT_VIEW_OBJECT_CUT_SEGMENT);
  _salomeModule->visibilityStateUpdateOn();
  _salomeModule->updateVisibilityState();
}

void
PresentationController::onDeletePresentation()
{
  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();

  // For each object, emit a signal to the workspace to request pres deletion
  for (int i=0; i<(int)listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soPres = listOfSObject->at(i);
      std::string name(_studyEditor->getName(soPres));
      if (soPres->_is_nil() || name == "MEDCalc")
        return;
      int presId = _salomeModule->getIntParamFromStudyEditor(soPres,PRESENTATION_ID);
      // If fieldId equals -1, then it means that it is not a field
      // managed by the MED module, and we stop this function process.
      if ( presId < 0 )
        continue;

      PresentationEvent* event = new PresentationEvent();
      event->eventtype = PresentationEvent::EVENT_DELETE_PRESENTATION;
      event->presentationId = presId;
      emit presentationSignal(event); // --> processPresentationEvent()
  }
}

QString
PresentationController::getViewModePython() const
{
  MEDCALC::ViewModeType viewMode = getSelectedViewMode();
  switch(viewMode) {
    case MEDCALC::VIEW_MODE_REPLACE: return "MEDCALC.VIEW_MODE_REPLACE";
    case MEDCALC::VIEW_MODE_OVERLAP: return "MEDCALC.VIEW_MODE_OVERLAP";
    case MEDCALC::VIEW_MODE_NEW_LAYOUT: return "MEDCALC.VIEW_MODE_NEW_LAYOUT";
    case MEDCALC::VIEW_MODE_SPLIT_VIEW: return "MEDCALC.VIEW_MODE_SPLIT_VIEW";
  }
  return QString();
}

QString
PresentationController::getColorMapPython() const
{
  MEDCALC::ColorMapType colorMap = getSelectedColorMap();
  switch(colorMap) {
    case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW: return "MEDCALC.COLOR_MAP_BLUE_TO_RED_RAINBOW";
    case MEDCALC::COLOR_MAP_COOL_TO_WARM: return "MEDCALC.COLOR_MAP_COOL_TO_WARM";
  }
  return QString();
}

QString
PresentationController::getScalarBarRangePython() const
{
  MEDCALC::ScalarBarRangeType scalarBarRange = getSelectedScalarBarRange();
  switch(scalarBarRange) {
    case MEDCALC::SCALAR_BAR_ALL_TIMESTEPS: return "MEDCALC.SCALAR_BAR_ALL_TIMESTEPS";
    case MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP: return "MEDCALC.SCALAR_BAR_CURRENT_TIMESTEP";
    case MEDCALC::SCALAR_BAR_CUSTOM_RANGE: return "MEDCALC.SCALAR_BAR_CUSTOM_RANGE";
  }
  return QString();
}

QString
PresentationController::getMeshModePython(const int mode) const
{
  MEDCALC::MeshModeType mod = static_cast<MEDCALC::MeshModeType>(mode);
  switch(mod) {
    case MEDCALC::MESH_MODE_WIREFRAME:     return "MEDCALC.MESH_MODE_WIREFRAME";
    case MEDCALC::MESH_MODE_SURFACE:       return "MEDCALC.MESH_MODE_SURFACE";
    case MEDCALC::MESH_MODE_SURFACE_EDGES: return "MEDCALC.MESH_MODE_SURFACE_EDGES";
  }
  return QString();
}

QString
PresentationController::getSliceOrientationPython(const int orientation) const
{
  MEDCALC::SliceOrientationType orient = static_cast<MEDCALC::SliceOrientationType>(orientation);
  switch(orient) {
    case MEDCALC::SLICE_NORMAL_TO_X:   return "MEDCALC.SLICE_NORMAL_TO_X";
    case MEDCALC::SLICE_NORMAL_TO_Y:   return "MEDCALC.SLICE_NORMAL_TO_Y";
    case MEDCALC::SLICE_NORMAL_TO_Z:   return "MEDCALC.SLICE_NORMAL_TO_Z";
    case MEDCALC::SLICE_NORMAL_TO_XY:  return "MEDCALC.SLICE_NORMAL_TO_XY";
    case MEDCALC::SLICE_NORMAL_TO_XZ:  return "MEDCALC.SLICE_NORMAL_TO_XZ";
    case MEDCALC::SLICE_NORMAL_TO_YZ:  return "MEDCALC.SLICE_NORMAL_TO_YZ";
    case MEDCALC::SLICE_NORMAL_TO_XYZ: return "MEDCALC.SLICE_NORMAL_TO_XYZ";
  }
  return QString();
}

QString
PresentationController::getIntegrDirTypePython(const int intDir) const
{
  MEDCALC::IntegrationDirType type = static_cast<MEDCALC::IntegrationDirType>(intDir);
  switch(type) {
    case MEDCALC::INTEGRATION_DIR_BOTH:     return "MEDCALC.INTEGRATION_DIR_BOTH";
    case MEDCALC::INTEGRATION_DIR_FORWARD:  return "MEDCALC.INTEGRATION_DIR_FORWARD";
    case MEDCALC::INTEGRATION_DIR_BACKWARD: return "MEDCALC.INTEGRATION_DIR_BACKWARD";
  }
  return QString();
}


std::string
PresentationController::getPresTypeFromWidgetHelper(int presId) const
{
  std::map<int, MEDWidgetHelper *>::const_iterator it =_presHelperMap.find(presId);
  if (it != _presHelperMap.end())
    return (*it).second->getPythonTag();
  return "UNKNOWN";
}

void
PresentationController::emitPresentationSignal(const PresentationEvent* event) 
{
  emit presentationSignal(event);
}

void
PresentationController::processPresentationEvent(const PresentationEvent* event) {
  // --> Send commands to SALOME Python console
  QString viewMode = getViewModePython();
  QString colorMap = getColorMapPython();
  QString scalarBarRange = getScalarBarRangePython();
  MEDCALC::FieldHandler* fieldHandler = event->fieldHandler;
  QStringList commands;

  // [ABN] using event mechanism for all this is awkward? TODO: direct implementation in each
  // dedicated widget helper class?

  if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_MESH_VIEW ) {
      // Do we request mesh view from a field or from a mesh only?
      int meshId = event->meshHandler ? event->meshHandler->id : event->fieldHandler->meshid;
      commands += QString("presentation_id = medcalc.MakeMeshView(%1, viewMode=%2)").arg(meshId).arg(viewMode);
      commands += QString("presentation_id");
    }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_SCALAR_MAP ) {
      commands += QString("presentation_id = medcalc.MakeScalarMap(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
          .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_CONTOUR ) {
      commands += QString("presentation_id = medcalc.MakeContour(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
            .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_SLICES ) {
      commands += QString("presentation_id = medcalc.MakeSlices(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
            .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_VECTOR_FIELD ) {
      commands += QString("presentation_id = medcalc.MakeVectorField(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
          .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_POINT_SPRITE ) {
      commands += QString("presentation_id = medcalc.MakePointSprite(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
              .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  // sphinx doc: begin of plot3d prs creation
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_PLOT3D ) {
      commands += QString("presentation_id = medcalc.MakePlot3D(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
              .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  // sphinx doc: end of plot3d prs creation
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_STREAM_LINES ) {
      commands += QString("presentation_id = medcalc.MakeStreamLines(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
              .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_CUT_SEGMENT ) {
      commands += QString("presentation_id = medcalc.MakeCutSegment(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
              .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }
  else if ( event->eventtype == PresentationEvent::EVENT_VIEW_OBJECT_DEFLECTION_SHAPE ) {
      commands += QString("presentation_id = medcalc.MakeDeflectionShape(accessField(%1), viewMode=%2, scalarBarRange=%3, colorMap=%4)")
          .arg(fieldHandler->id).arg(viewMode).arg(scalarBarRange).arg(colorMap);
      commands += QString("presentation_id");
  }


  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_COMPONENT ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
      commands += QString("params.displayedComponent = '%1'").arg(QString::fromStdString(event->aString));
      commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_COLORMAP ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
      commands += QString("params.colorMap = %1").arg(getColorMapPython());
      commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_TIME_RANGE ) {
      bool customRangeFlag =
        (bool)_presManager->getPresentationIntProperty(event->presentationId, MEDPresentation::PROP_HIDE_DATA_OUTSIDE_CUSTOM_RANGE.c_str());
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
      commands += QString("params.scalarBarRange = %1").arg(getScalarBarRangePython());
      if (getSelectedScalarBarRange() == MEDCALC::SCALAR_BAR_CUSTOM_RANGE) {
        commands += QString("params.scalarBarRangeArray = [%1, %2]").arg(event->aDouble1).arg(event->aDouble2);
      }
      else {
        if (customRangeFlag) // switch off hideDataOutsideCustomRange
        {
          commands += QString("params.hideDataOutsideCustomRange = False");
        }
      }
      commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_NB_CONTOUR ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetContourParameters(%2)").arg(event->presentationId);
      commands += QString("params.nbContours = %1").arg(event->anInteger);
      commands += QString("medcalc.UpdateContour(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_MESH_MODE ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetMeshViewParameters(%2)").arg(event->presentationId);
      commands += QString("params.mode = %1").arg(getMeshModePython(event->anInteger));
      commands += QString("medcalc.UpdateMeshView(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_NB_SLICES ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetSlicesParameters(%2)").arg(event->presentationId);
      commands += QString("params.nbSlices = %1").arg(event->anInteger);
      commands += QString("medcalc.UpdateSlices(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_SLICE_ORIENTATION ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetSlicesParameters(%2)").arg(event->presentationId);
      commands += QString("params.orientation = %1").arg(getSliceOrientationPython(event->anInteger));
      commands += QString("medcalc.UpdateSlices(%1, params)").arg(event->presentationId);
  }
  // sphinx doc: begin of plot3d prs update
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_PLANE_POS ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetPlot3DParameters(%2)").arg(event->presentationId);
      commands += QString("params.planePos = %1").arg(event->aDouble3);
      commands += QString("medcalc.UpdatePlot3D(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_NORMAL ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetPlot3DParameters(%2)").arg(event->presentationId);
      commands += QString("params.planeNormal = [%1, %2, %3]").arg(event->aDoubleN[0]).arg(event->aDoubleN[1]).arg(event->aDoubleN[2]);
      commands += QString("medcalc.UpdatePlot3D(%1, params)").arg(event->presentationId);
  }
  // sphinx doc: end of plot3d prs update
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_CUT_POINT1 ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetCutSegmentParameters(%2)").arg(event->presentationId);
      commands += QString("params.point1 = [%1, %2, %3]").arg(event->aDoubleP1[0]).arg(event->aDoubleP1[1]).arg(event->aDoubleP1[2]);
      commands += QString("medcalc.UpdateCutSegment(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_CUT_POINT2 ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetCutSegmentParameters(%2)").arg(event->presentationId);
      commands += QString("params.point2 = [%1, %2, %3]").arg(event->aDoubleP2[0]).arg(event->aDoubleP2[1]).arg(event->aDoubleP2[2]);
      commands += QString("medcalc.UpdateCutSegment(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_INTEGR_DIR ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetStreamLinesParameters(%2)").arg(event->presentationId);
      commands += QString("params.integrDir = %1").arg(getIntegrDirTypePython(event->anInteger));
      commands += QString("medcalc.UpdateStreamLines(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_CONTOUR_COMPONENT) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetContourParameters(%2)").arg(event->presentationId);
      commands += QString("params.contourComponent = '%1'").arg(event->aString.c_str());
      commands += QString("medcalc.UpdateContour(%1, params)").arg(event->presentationId);
  }
  else if ( event->eventtype == PresentationEvent::EVENT_CHANGE_SCALE_FACTOR ) {
      std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
      commands += QString("params = medcalc.GetVectorFieldParameters(%2)").arg(event->presentationId);
      commands += QString("params.scaleFactor = %1").arg(event->aDouble3);
      commands += QString("medcalc.UpdateVectorField(%1, params)").arg(event->presentationId);
  }

  else if (event->eventtype == PresentationEvent::EVENT_CHANGE_CUSTOM_SCALE_FACTOR) {
  std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
  commands += QString("params = medcalc.GetVectorFieldParameters(%2)").arg(event->presentationId);
  commands += QString("params.customScaleFactor = %1").arg(event->anInteger);
  if(event->anInteger) {
    commands += QString("params.scaleFactor = %1").arg(event->aDouble3);
  }
  commands += QString("medcalc.UpdateVectorField(%1, params)").arg(event->presentationId);
  }

  else if ( event->eventtype == PresentationEvent::EVENT_DELETE_PRESENTATION ) {
      commands += QString("medcalc.RemovePresentation(%1)").arg(event->presentationId);

  }
  else if (event->eventtype == PresentationEvent::EVENT_DISPLAY_PRESENTATION ||
           event->eventtype == PresentationEvent::EVENT_ERASE_PRESENTATION) {
    commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(getPresTypeFromWidgetHelper(event->presentationId)))
      .arg(event->presentationId);
    QString visility = event->eventtype == PresentationEvent::EVENT_DISPLAY_PRESENTATION ? QString("True") : QString("False");
    commands += QString("params.visibility = %1").arg(visility);
    commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(getPresTypeFromWidgetHelper(event->presentationId)))
      .arg(event->presentationId);
  }
  else if (event->eventtype == PresentationEvent::EVENT_SCALAR_BAR_VISIBILITY_CHANGED ||
           event->eventtype == PresentationEvent::EVENT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE_CHANGED) {
    std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
    QString param;
    switch (event->eventtype) {
    case PresentationEvent::EVENT_SCALAR_BAR_VISIBILITY_CHANGED: 
      param = QString("scalarBarVisibility"); 
      break;
    case PresentationEvent::EVENT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE_CHANGED:
      param = QString("hideDataOutsideCustomRange"); 
      break;
    default: break;
    }
    commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
    commands += QString("params.%1 = %2").arg(param).arg( event->anInteger ? QString("True") : QString("False"));
    if (event->eventtype == PresentationEvent::EVENT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE_CHANGED && event->anInteger) {
      commands += QString("params.scalarBarRangeArray = [%1, %2]").arg(event->aDouble1).arg(event->aDouble2);
    }
    commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
  }
  else if (event->eventtype == PresentationEvent::EVENT_CUSTOM_RANGE_CHANGED)
  {
  std::string typ = getPresTypeFromWidgetHelper(event->presentationId);
    commands += QString("params = medcalc.Get%1Parameters(%2)").arg(QString::fromStdString(typ)).arg(event->presentationId);
    commands += QString("params.scalarBarRangeArray = [%1, %2]").arg(event->aDouble1).arg(event->aDouble2);
    commands += QString("medcalc.Update%1(%2, params)").arg(QString::fromStdString(typ)).arg(event->presentationId);
  }
  else {
      STDLOG("The event "<<event->eventtype<<" is not implemented yet");
  }
  _consoleDriver->exec(commands);
}

MEDWidgetHelper *
PresentationController::findOrCreateWidgetHelper(MEDCALC::MEDPresentationManager_ptr /*presManager*/,                  // todo: unused
                                                 int presId, const std::string& type, const std::string& name )
{
  std::map<int, MEDWidgetHelper *>::const_iterator it =_presHelperMap.find(presId);
  if (it != _presHelperMap.end())
    return (*it).second;
  MEDWidgetHelper * wh = 0;
  if (type == MEDPresentationMeshView::TYPE_NAME)
    wh = new MEDWidgetHelperMeshView(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationScalarMap::TYPE_NAME)
    wh = new MEDWidgetHelperScalarMap(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationContour::TYPE_NAME)
    wh = new MEDWidgetHelperContour(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationSlices::TYPE_NAME)
    wh = new MEDWidgetHelperSlices(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationVectorField::TYPE_NAME)
    wh = new MEDWidgetHelperVectorField(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationPointSprite::TYPE_NAME)
    wh = new MEDWidgetHelperPointSprite(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationPlot3D::TYPE_NAME)
    wh = new MEDWidgetHelperPlot3D(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationStreamLines::TYPE_NAME)
    wh = new MEDWidgetHelperStreamLines(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationCutSegment::TYPE_NAME)
    wh = new MEDWidgetHelperCutSegment(this, _presManager, presId, name, _widgetPresentationParameters);
  else if (type == MEDPresentationDeflectionShape::TYPE_NAME)
    wh = new MEDWidgetHelperDeflectionShape(this, _presManager, presId, name, _widgetPresentationParameters);
  else
    {
      STDLOG("findOrCreateWidgetHelper(): NOT IMPLEMENTED !!!");
      return wh;
    }
  _presHelperMap[presId] = wh;
  return wh;
}

void
PresentationController::onPresentationSelected(int presId, const QString& presType, const QString& presName)
{
  if (presId == -1)
    {
      if (_widgetPresentationParameters->isShown())
        {
          _widgetPresentationParameters->toggleWidget(false);
          if(_currentWidgetHelper)
            _currentWidgetHelper->releaseWidget();
        }
    }
  else
    {
      if(_currentWidgetHelper)
        _currentWidgetHelper->releaseWidget();
      // Activate corresponding ParaView render view
      _presManager->activateView(presId);
      // Update widgets parameters
      _currentWidgetHelper = findOrCreateWidgetHelper(_presManager, presId, presType.toStdString(), presName.toStdString());
      _currentWidgetHelper->updateWidget(true);
    }
}

void
PresentationController::onParavisDump()
{
  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();

  // For the first object only, request the dump
  for (int i=0; i<(int)listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soPres = listOfSObject->at(i);
      std::string name(_studyEditor->getName(soPres));
      if (soPres->_is_nil() || name == "MEDCalc")
        return;
      int presId = _salomeModule->getIntParamFromStudyEditor(soPres,PRESENTATION_ID);
      // If fieldId equals -1, then it means that it is not a field
      // managed by the MED module, and we stop this function process.
      if ( presId < 0 )
        continue;

      std::string dump(_presManager->getParavisDump(presId));
      std::cerr << "#====== ParaVis dump (presentation "  << presId << ") =====" << std::endl;
      std::cerr << dump;
      std::cerr << "#====== End of ParaVis dump =============== " << std::endl;

      break; // stop at the first one
  }
}

void
PresentationController::updateTreeViewWithNewPresentation(long dataId, long presentationId)
{
  if (presentationId < 0) {
      std::cerr << "Unknown presentation\n";
      return;
  }

  std::string name(_presManager->getPresentationStringProperty(presentationId, MEDPresentation::PROP_NAME.c_str()));
  std::string type = name;
  std::string icon = std::string("ICO_") + type;
  icon = _getIconName(icon);
  std::string ico = tr(icon.c_str()).toStdString();

  // Append presentation ID to the displayed name in the OB:
  std::ostringstream oss;
  name = tr(name.c_str()).toStdString();
  oss << name << " (" << presentationId << ")";

  // Mesh views are always registered at the mesh level:
  if (type == MEDPresentationMeshView::TYPE_NAME)
    {
      _salomeModule->engine()->registerPresentationMesh(dataId, oss.str().c_str(), type.c_str(),ico.c_str(), presentationId);
    }
  else
    _salomeModule->engine()->registerPresentationField(dataId, oss.str().c_str(), type.c_str(),ico.c_str(), presentationId);

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);

  // auto-select new presentation
  std::string entry = _salomeModule->engine()->getStudyPresentationEntry(presentationId);
  SALOME_ListIO selectedObjects;
  LightApp_Study* lightStudy = dynamic_cast<LightApp_Study*>( _salomeModule->application()->activeStudy() );
  QString component = lightStudy->componentDataType( entry.c_str() );
  selectedObjects.Append( new SALOME_InteractiveObject( (const char*)entry.c_str(),
                                                        (const char*)component.toLatin1(),
                                                        ""/*refobj->Name().c_str()*/ ) );
  //QStringList selectedObjects;
  //selectedObjects << QString(entry.c_str());
  LightApp_SelectionMgr* aSelectionMgr = _salomeModule->getApp()->selectionMgr();
  aSelectionMgr->setSelectedObjects(selectedObjects, false);

  // emit onPresentationSelected
  int presId = -1;
  _salomeModule->itemClickGeneric(name, type, presId);
  onPresentationSelected(presId, QString::fromStdString(type), QString::fromStdString(name));
  updateVisibilityState(presId);
}

void
PresentationController::updateTreeViewForPresentationRemoval(long presentationId)
{
  if (presentationId < 0) {
      std::cerr << "Unknown presentation\n";
      return;
  }

  _salomeModule->engine()->unregisterPresentation(presentationId);

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);
}

void
PresentationController::_dealWithReplaceMode()
{
  // Deal with replace mode: presentations with invalid IDs have to be removed:

  MEDCALC::PresentationsList * lstManager = _presManager->getAllPresentations();
  MED_ORB::PresentationsList * lstModule = _salomeModule->engine()->getStudyPresentations();
  // The IDs not in the intersection needs deletion:
  CORBA::Long * last = lstManager->get_buffer() + lstManager->length();
  for (unsigned i = 0; i < lstModule->length(); i++) {
    CORBA::Long * ptr = std::find(lstManager->get_buffer(), last, (*lstModule)[i]);
    if (ptr == last) {
      STDLOG("Removing pres " << (*lstModule)[i] << " from OB.");
      // Presentation in module but not in manager anymore: to be deleted from OB:
      updateTreeViewForPresentationRemoval((*lstModule)[i]);
    }
  }
}

void
PresentationController::processWorkspaceEvent(const MEDCALC::MedEvent* event)
{
  if ( event->type == MEDCALC::EVENT_ADD_PRESENTATION ) {
    if (event->dataId == -1) {
      // A file has been loaded, and we want to create a default presentation (MeshView) for it
      QString viewMode = getViewModePython();
      QStringList commands;
      commands += QString("presentation_id = medcalc.MakeMeshView(medcalc.GetFirstMeshFromDataSource(source_id), viewMode=%1)").arg(viewMode);
      commands += QString("presentation_id");
      _consoleDriver->exec(commands);
    }
    else {
      updateTreeViewWithNewPresentation(event->dataId, event->presentationId);
      _dealWithReplaceMode();
      // Update parameter widget if shown: some parameters should be updated after presentation has been added
      if (_currentWidgetHelper)
        _currentWidgetHelper->updateWidget(false);
    }
  }
  else if ( event->type == MEDCALC::EVENT_REMOVE_PRESENTATION ) {
      updateTreeViewForPresentationRemoval(event->presentationId);
      // Hide parameter widget if necessary:
      onPresentationSelected(-1, "", "");
  }
  else if ( event->type == MEDCALC::EVENT_MODIFY_PRESENTATION ) {
      // Update parameter widget if shown:
      if(_currentWidgetHelper)
        _currentWidgetHelper->updateWidget(false);
  }
  else if (event->type == MEDCALC::EVENT_VISIBILITY_CHANGED) {
    updateVisibilityState(event->presentationId);
  }
}

void
PresentationController::showDockWidgets(bool isVisible)
{
  _dockWidget->setVisible(isVisible);
}

void PresentationController::updateVisibilityState(long presId) 
{
  char* str = _salomeModule->engine()->getStudyPresentationEntry(presId);
  if (str) {
    QStringList entries;
    entries.append(str);
    _salomeModule->updateVisibilityState(false, entries);
  }
}

std::string PresentationController::presentationName2Type(const std::string& name) {
  return std::regex_replace(name, std::regex("MEDPresentation"), std::string(""));
}

void PresentationController::resetPVSession()
{
  QStringList commands;
  commands += QString("pvs.ResetSession()");
  _consoleDriver->exec(commands);
}
