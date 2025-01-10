// Copyright (C) 2016-2025  CEA, EDF
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

#include "ProcessingController.hxx"
#include <MEDCalcConstants.hxx>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include "MEDFactoryClient.hxx"
#include "MEDModule.hxx"
#include "Basics_Utils.hxx"

#include <SUIT_Desktop.h>
#include <QMessageBox>

ProcessingController::ProcessingController(MEDModule* salomeModule)
{
  STDLOG("Creating a ProcessingController");
  _salomeModule = salomeModule;
  _studyEditor = _salomeModule->getStudyEditor();

  _dlgChangeUnderlyingMesh = new DlgChangeUnderlyingMesh(_studyEditor);
  connect(_dlgChangeUnderlyingMesh,SIGNAL(inputValidated()),
          this, SLOT(OnChangeUnderlyingMeshInputValidated()));

  _dlgInterpolateField = new DlgInterpolateField(_studyEditor);
  connect(_dlgInterpolateField,SIGNAL(inputValidated()),
          this, SLOT(OnInterpolateFieldInputValidated()));
}

ProcessingController::~ProcessingController()
{
  STDLOG("Deleting the ProcessingController");
}

void
ProcessingController::createActions()
{
  STDLOG("Creating ProcessingController actions");

  int processingToolbarId = _salomeModule->createTool("Processing", "ProcessingToolbar");
  int processingMenuId = _salomeModule->createMenu(tr("MENU_PROCESSING"), -1, -1, 12);

  // Change underlying mesh (note that this action creates a new field in
  // the workspace that corresponds to a copy of the selected field
  // modified by the change of the underlying mesh.
  QString label   = tr("LAB_PROCESSING_CHANGE_MESH");
  QString icon = tr("ICO_PROCESSING_CHANGE_MESH");
  int actionId;
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnChangeUnderlyingMesh()),icon,label);
  _salomeModule->createTool(actionId, processingToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, processingMenuId);

  label   = tr("LAB_PROCESSING_INTERPOLATE_FIELD");
  icon = tr("ICO_PROCESSING_INTERPOLATE_FIELD");
  actionId = _salomeModule->createStandardAction(label, this, SLOT(OnInterpolateField()),
                                                 icon, label, FIELDSOp::OpProcessingInterpolation);
  _salomeModule->createTool(actionId, processingToolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, processingMenuId);
}

void
ProcessingController::OnChangeUnderlyingMesh()
{
  // Get the selected objects in the study (SObject). In case of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soObj = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soObj));
    if (soObj->_is_nil() || name == "MEDCalc")
      return;

    int fieldId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_ID);
    if (fieldId < 0) { // is it a field series?
      int fieldSeriesId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_SERIES_ID);
      // If fieldId and fieldSeriesId equals -1, then it means that it is not a field
      // managed by the MED module, and we stop this function process.
      if ( fieldSeriesId < 0)
        return;
      MEDCALC::FieldHandlerList* fieldHandlerList = MEDFactoryClient::getDataManager()->getFieldListInFieldseries(fieldSeriesId);
      /* todo: always false!
      if (fieldHandlerList->length() < 0)
        return;
      */
      // For a field series, get the first real field entry:
      MEDCALC::FieldHandler fieldHandler = (*fieldHandlerList)[0];
      fieldId = fieldHandler.id;
      if (fieldId < 0)
        return;
    }

    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgChangeUnderlyingMesh->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgChangeUnderlyingMesh->windowFlags();
    _dlgChangeUnderlyingMesh->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgChangeUnderlyingMesh->open();
  }
}

void
ProcessingController::OnChangeUnderlyingMeshInputValidated()
{
  STDLOG("Change Underlying Mesh");
  int meshId = _dlgChangeUnderlyingMesh->getMeshId();
  STDLOG("meshId = " << ToString(meshId));
  int fieldId = _dlgChangeUnderlyingMesh->getFieldId();

  ProcessingEvent* event = new ProcessingEvent();
  event->eventtype = ProcessingEvent::EVENT_CHANGE_UNDERLYING_MESH;
  event->fieldId = fieldId;
  event->meshId = meshId;
  emit processingSignal(event); // --> WorkspaceController::processProcessingEvent()

  // Tag the item to prevent double import
  //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
}

void
ProcessingController::OnInterpolateField()
{
  // Get the selected objects in the study (SObject). In case of a
  // multiple selection, we consider only the first item. At least one
  // item must be selected.
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soObj = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soObj));
    if (soObj->_is_nil() || name == "MEDCalc")
      return;

    int fieldId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_ID);
    if (fieldId < 0) { // is it a field series?
      int fieldSeriesId = _salomeModule->getIntParamFromStudyEditor(soObj, FIELD_SERIES_ID);
      // If fieldId and fieldSeriesId equals -1, then it means that it is not a field
      // managed by the MED module, and we stop this function process.
      if ( fieldSeriesId < 0)
        return;
      MEDCALC::FieldHandlerList* fieldHandlerList = MEDFactoryClient::getDataManager()->getFieldListInFieldseries(fieldSeriesId);
      /* todo: always false!
      if (fieldHandlerList->length() < 0)
        return;
      */
      // For a field series, get the first real field entry:
      MEDCALC::FieldHandler fieldHandler = (*fieldHandlerList)[0];
      fieldId = fieldHandler.id;
      if (fieldId < 0)
        return;
    }

    // _GBO_ : the dialog should not be modal, so that we can choose a
    // mesh in the browser. Then we have to emit a signal from the
    // dialog.accept, connected to a slot of the DatasourceControler
    _dlgInterpolateField->setFieldId(fieldId);
    Qt::WindowFlags flags = _dlgInterpolateField->windowFlags();
    _dlgInterpolateField->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    _dlgInterpolateField->open();
  }
}

void
ProcessingController::OnInterpolateFieldInputValidated()
{
  STDLOG("Interpolate Field");
  MEDCALC::InterpolationParameters params;
  params.precision = _dlgInterpolateField->getPrecision();
  STDLOG("precision = " << params.precision);
  params.defaultValue = _dlgInterpolateField->getDefaultValue();
  STDLOG("defaultValue = " << params.defaultValue);
  params.reverse = _dlgInterpolateField->getReverse();
  STDLOG("reverse = " << params.reverse);
  params.intersectionType = _dlgInterpolateField->getIntersectionType().c_str();
  STDLOG("intersectionType = " << params.intersectionType);
  params.method = _dlgInterpolateField->getMethod().c_str();
  STDLOG("method = " << params.method);
  params.nature = _dlgInterpolateField->getFieldNature().c_str();
  STDLOG("nature = " << params.nature);

  int meshId = _dlgInterpolateField->getMeshId();
  STDLOG("meshId = " << ToString(meshId));
  int fieldId = _dlgInterpolateField->getFieldId();

  ProcessingEvent* event = new ProcessingEvent();
  event->eventtype = ProcessingEvent::EVENT_INTERPOLATE_FIELD;
  event->fieldId = fieldId;
  event->meshId = meshId;
  event->interpParams = params;
  emit processingSignal(event); // --> WorkspaceController::processProcessingEvent()

  // // Tag the item to prevent double import
  // //_studyEditor->setParameterBool(soField,OBJECT_IS_IN_WORKSPACE,true);
}
