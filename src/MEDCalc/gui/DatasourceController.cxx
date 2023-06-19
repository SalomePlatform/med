// Copyright (C) 2007-2023  CEA, EDF
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

// Author : Guillaume Boulant (EDF)

#include "DatasourceController.hxx"
#include <MEDCalcConstants.hxx>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataObject.h>

#include <SALOME_ListIO.hxx>
#include <LightApp_SelectionMgr.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>

#include "MEDFactoryClient.hxx"
#include "MEDModule.hxx"
#include "QtHelper.hxx"

#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>

#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>

#include "DlgAlias.hxx"

//
// ==============================================================
// Datasource controller
// ==============================================================
//
DatasourceController::DatasourceController(MEDModule* salomeModule)
{
  STDLOG("Creating a DatasourceController");
  _salomeModule = salomeModule;
  _studyEditor = _salomeModule->getStudyEditor();
}

DatasourceController::~DatasourceController() {
  STDLOG("Deleting the DatasourceController");
}

void DatasourceController::createActions() {
  int toolbarId = _salomeModule->createTool("Datasource", "DatasourceToolbar");

  //
  // Main actions (toolbar and menubar)
  //
  QString label   = tr("LAB_ADD_DATA_SOURCE");
  QString tooltip = tr("TIP_ADD_DATA_SOURCE");
  QString icon    = tr("ICO_DATASOURCE_ADD");
  int actionId;
  actionId = _salomeModule->createStandardAction(label, this, SLOT(OnAddDatasource()),
                                                 icon, tooltip, FIELDSOp::OpAddDataSource);
  _salomeModule->createTool(actionId, toolbarId);

  // This action has to be placed in the general file menu with the label "Import MED file"
  int menuId = _salomeModule->createMenu( tr( "MEN_FILE" ), -1,  1 );
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, menuId, 10);

  label   = tr("LAB_ADD_IMAGE_SOURCE");
  tooltip = tr("TIP_ADD_IMAGE_SOURCE");
  icon    = tr("ICO_IMAGE_ADD");
  actionId = _salomeModule->createStandardAction(label,this, SLOT(OnAddImagesource()),icon,tooltip);
  _salomeModule->createTool(actionId, toolbarId);
  _salomeModule->action(actionId)->setIconVisibleInMenu(true);
  _salomeModule->createMenu(actionId, menuId, 20);

  //
  // Actions for popup menu only
  //
  // Expand field timeseries
  label = tr("LAB_EXPAND_FIELD");
  icon  = tr("ICO_DATASOURCE_EXPAND_FIELD");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnExpandField()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

  // Use in workspace
  label = tr("LAB_USE_IN_WORKSPACE");
  icon  = tr("ICO_DATASOURCE_USE");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnUseInWorkspace()),icon);
  _salomeModule->addActionInPopupMenu(actionId);

}

/**
 * This function adds the specified MED file as a datasource in the
 * dataspace. Technically speaking, the engine loads the
 * meta-information concerning med data from the file, gives this
 * information to the GUI, and the GUI creates a tree view of these
 * data in the study object browser.
 */
// This function emits a signal that will be caught by workspace to delegate command (datasource creation) to python console.
void
DatasourceController::addDatasource(const char* filename)
{
  DatasourceEvent* event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_ADD_DATASOURCE;
  event->objectalias = filename;
  emit datasourceSignal(event);  // --> WorkspaceController::processDatasourceEvent()
//#ifdef MED_WITH_QTTESTING
//  _dirtyAddDataSource = true;
//  while(_dirtyAddDataSource)
//    QApplication::processEvents();
//#endif
}

// After above data source creation, python console emits a signal, forwarded by workspace, to update the GUI
void
DatasourceController::updateTreeViewWithNewDatasource(const MEDCALC::DatasourceHandler* datasourceHandler)
{
  if (!datasourceHandler) {
    return;
  }

  _salomeModule->engine()->addDatasourceToStudy(*datasourceHandler);

  // update Object browser
  _salomeModule->getApp()->updateObjectBrowser(true);

//#ifdef MED_WITH_QTTESTING
//  _dirtyAddDataSource = false;
//#endif
}

void DatasourceController::OnAddDatasource()
{
  // Dialog to get the filename where the input data are read from
  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));

  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();

//  QStringList filenames = SUIT_FileDlg::getOpenFileNames( _salomeModule->getApp()->desktop(),
//                                                          anInitialPath,
//                                                          filter,
//                                                          tr("IMPORT_MED_FIELDS") );
  // [ABN] the below to be compatible with QtTesting:
  QStringList filenames = QFileDialog::getOpenFileNames( _salomeModule->getApp()->desktop(),
                                                          tr("IMPORT_MED_FIELDS"),
                                                          anInitialPath,
                                                          tr("FILE_FILTER_MED") );

  if ( filenames.count() <= 0 ) return;
  for ( QStringList::ConstIterator itFile = filenames.begin();
        itFile != filenames.end(); ++itFile ) {
    QString filename = *itFile;
    this->addDatasource(QCHARSTAR(filename));
    _salomeModule->updateObjBrowser(true);
  }
}

#include "DlgImageToMed.hxx"
void DatasourceController::OnAddImagesource()
{

  DlgImageToMed dialog;
  dialog.setAutoLoaded(true);
  int choice = dialog.exec();
  if ( choice == QDialog::Rejected ) {
    // The user decides to cancel the operation
    return;
  }

  QString imageFilename = dialog.getImageFilepath();
  /*
  QString medFilename   = dialog.getMedFilepath();
  bool autoLoad         = dialog.isAutoLoaded();

  std::string ROOT_DIR(getenv("FIELDS_ROOT_DIR"));
  std::string command(ROOT_DIR+"/bin/salome/fields/image2med.py");
  command += " -i "+QS2S(imageFilename);
  command += " -m "+QS2S(medFilename);
  int error = system(command.c_str());
  if ( error != 0 ) {
    QMessageBox::critical(_salomeModule->getApp()->desktop(),
           tr("Operation failed"),
           tr("The creation of med data from the image file failed"));
    return;
  }

  if ( autoLoad ) {
    this->addDatasource(QCHARSTAR(medFilename));
    _salomeModule->updateObjBrowser(true);
  }
  */

  DatasourceEvent* event = new DatasourceEvent();
  event->eventtype = DatasourceEvent::EVENT_ADD_IMAGE_AS_DATASOURCE;
  event->objectalias = imageFilename;
  emit datasourceSignal(event); // --> WorkspaceController::processDatasourceEvent()
}

void DatasourceController::OnExpandField()
{
  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  // check if reference to study is valid
  if (CORBA::is_nil(aStudy)) 
    return;

  // check if reference to use case builder is valid
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();
  if (CORBA::is_nil(useCaseBuilder)) 
    return;


  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  for (int i=0; i<(int)listOfSObject->size(); i++) {
    SALOMEDS::SObject_var soFieldseries = listOfSObject->at(i);
    std::string name(_studyEditor->getName(soFieldseries));
    if (soFieldseries->_is_nil() || name == "MEDCalc")
      return;

    // First retrieve the fieldseries id associated to this study object
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if ( !soFieldseries->FindAttribute(anAttr,"AttributeParameter") )
      return;
    aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
    if (! aParam->IsSet(FIELD_SERIES_ID, PT_INTEGER))
      return;
    long fieldseriesId = aParam->GetInt(FIELD_SERIES_ID);
    STDLOG("Expand the field timeseries "<<fieldseriesId);

    // If fieldseriesId equals -1, then it means that it is not a
    // fieldseries managed by the MED module, and we stop this
    // function process.
    if ( fieldseriesId < 0 )
      continue;
    // _GBO_ A better correction should be to no display the
    // contextual menu if the selected object is not conform

    // Then retrieve the list of fields in this timeseries
    MEDCALC::FieldHandlerList* fieldHandlerList =
      MEDFactoryClient::getDataManager()->getFieldListInFieldseries(fieldseriesId);

    // Finally, create an entry for each of the field
    for(CORBA::ULong iField=0; iField<fieldHandlerList->length(); iField++) {
      MEDCALC::FieldHandler fieldHandler = (*fieldHandlerList)[iField];
      SALOMEDS::SObject_var soField = _studyEditor->newObject(soFieldseries);
      std::string label("it="); label += ToString(fieldHandler.iteration);
      _studyEditor->setName(soField,label.c_str());
      _studyEditor->setParameterInt(soField, FIELD_ID, fieldHandler.id);
      _studyEditor->setParameterBool(soField,IS_IN_WORKSPACE,false);
      useCaseBuilder->AppendTo(soField->GetFather(), soField);
    }
  }
  _salomeModule->updateObjBrowser(true);
}

void DatasourceController::OnUseInWorkspace() {
  // Get the selected objects in the study (SObject)
  SALOME_StudyEditor::SObjectList* listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() == 1 ) {
    // In this case we ask the name of the variable for the python
    // console

    // >>>
    // _GBO_ Note that it works only for a single field but the
    // XmedDataObject will be improved to deal with mesh, timeseries
    // and single field in a futur version. We suppose here that a
    // single field has been selected.
    // <<<

    SALOMEDS::SObject_var soField = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soField));
    if (soField->_is_nil() || name == "MEDCalc")
      return;
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if ( !soField->FindAttribute(anAttr,"AttributeParameter") )
      return;
    aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
    if (! aParam->IsSet(IS_IN_WORKSPACE, PT_BOOLEAN))
      return;
    bool isInWorkspace = aParam->GetBool(IS_IN_WORKSPACE);
    if ( isInWorkspace ) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("This field is already defined in the workspace"));
      return;
    }

    if (! aParam->IsSet(FIELD_ID, PT_INTEGER))
      return;
    int fieldId = aParam->GetInt(FIELD_ID);

    // If fieldId equals -1, then it means that it is not a field
    // managed by the MED module, and we stop this function process.
    if ( fieldId < 0 ) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("This element is not a field object"));
      return;
    }

    MEDCALC::FieldHandler* fieldHandler =
      MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);

    if (! fieldHandler) {
      QMessageBox::warning(_salomeModule->getApp()->desktop(),
         tr("Operation not allowed"),
         tr("No field is defined"));
      return;
    }

    QString alias(fieldHandler->fieldname);
    DlgAlias dialog;
    dialog.setAlias(alias);
    int choice = dialog.exec();
    if ( choice == QDialog::Rejected ) {
      // The user decides to cancel the operation
      return;
    }
    alias = dialog.getAlias();

    DatasourceEvent* event = new DatasourceEvent();
    event->eventtype = DatasourceEvent::EVENT_USE_OBJECT;
    XmedDataObject* dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    event->objectdata  = dataObject;
    event->objectalias = alias;
    emit datasourceSignal(event);  // --> WorkspaceController::processDatasourceEvent()
    // Tag the item to prevent double import
    //    _studyEditor->setParameterBool(soField,IS_IN_WORKSPACE,true);
    // Tag the field as persistent on the server. It means that a
    // saving of the workspace will save at least this field (maybe it
    // should be an option?)
    MEDFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
  }
  else {
    // In this case, we don't ask the user to specify an alias for
    // each item, we just import the whole set of items.
    for (int i=0; i<(int)listOfSObject->size(); i++) {
      SALOMEDS::SObject_var soField = listOfSObject->at(i);
      if (soField->_is_nil())
        continue;
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeParameter_var aParam;
      if ( !soField->FindAttribute(anAttr,"AttributeParameter") )
        return;
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (! aParam->IsSet(IS_IN_WORKSPACE, PT_BOOLEAN))
        return;
      bool isInWorkspace = aParam->GetBool(IS_IN_WORKSPACE);
      if ( !isInWorkspace ) {
        if (! aParam->IsSet(FIELD_ID, PT_INTEGER))
          continue;
        int fieldId = aParam->GetInt(FIELD_ID);
        MEDCALC::FieldHandler* fieldHandler =
          MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
        DatasourceEvent* event = new DatasourceEvent();
        event->eventtype = DatasourceEvent::EVENT_IMPORT_OBJECT;
        XmedDataObject* dataObject = new XmedDataObject();
        dataObject->setFieldHandler(*fieldHandler);
        event->objectdata  = dataObject;
        emit datasourceSignal(event); // --> WorkspaceController::processDatasourceEvent()
        // Note that this signal is processed by the WorkspaceController

        // Tag the item to prevent double import
        //        _studyEditor->setParameterBool(soField,IS_IN_WORKSPACE,true);
        // Tag the field as persistent on the server. It means that a
        // saving of the workspace will save at least this field (maybe it
        // should be an option?)
        MEDFactoryClient::getDataManager()->markAsPersistent(fieldId, true);
      }
      else {
        STDLOG("The field "<<_studyEditor->getName(soField)<<
               " is already defined in the workspace");
      }
    }
  }
}

void
DatasourceController::processWorkspaceEvent(const MEDCALC::MedEvent* event)
{
  if ( event->type == MEDCALC::EVENT_ADD_DATASOURCE ) {
    MEDCALC::DatasourceHandler* datasourceHandler = MEDFactoryClient::getDataManager()->getDatasourceHandler(event->filename);
    this->updateTreeViewWithNewDatasource(datasourceHandler);
  }
}
