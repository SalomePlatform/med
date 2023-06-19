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

#include "WorkspaceController.hxx"
#include "QtHelper.hxx"
#include "MEDFactoryClient.hxx"
#include "MEDModule.hxx"
#include "XmedDataModel.hxx"
#include "DlgAlias.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDEventListener)

#include <SalomeApp_Application.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>

#include <QTimer>
#include <QMessageBox>

#include "MEDLoader.hxx"
using namespace MEDCoupling;
#include "MEDDataManager_i.hxx"

/*!
 * This class defines a DockWidget plugged in the SALOME application,
 * and containing a tree view for rendering a hierarchical data
 * model. This datamodel contains the objects used in the workspace.
 */
WorkspaceController::WorkspaceController(MEDModule* salomeModule)
  : TreeGuiManager(salomeModule->getApp(), "Workspace")
{
  _salomeModule = salomeModule;
  getDockWidgets()->getDockWidget()->setObjectName("medWorkspaceDock");

  this->tabifyDockWidgets(false);

  // -------------------------------------------------------------
  // Setup the MEDEventListener to manage notification from the
  // python console.

  // We create a MEDEventListener CORBA object inside this GUI class
  // with the role of listening events coming from the python console
  // (or even the components if needed). The events arising in the
  // python console are send as CORBA request to this CORBA
  // servant. Then this object can process the event by notifying the
  // GUI of something to update for example (using signals and slots
  // of course).
  _medEventListener = MEDEventListener_i::getInstance();
  MEDCALC::MEDEventListener_ptr medEventListenerServant = _medEventListener->_this();

  // We store the IOR inside the MEDDataManager to share this data
  // with other parts of the application, in particular the python
  // console that could retrieve this IOR using the
  // getEventListenerIOR() function of the MEDDataManager.
  SalomeApp_Application* salomeApp = salomeModule->getApp();
  const char* medEventListenerIOR =
    salomeApp->orb()->object_to_string(medEventListenerServant);
  MEDFactoryClient::getDataManager()->setEventListenerIOR(medEventListenerIOR);

  // Connect the signals emitted from the MEDEventListener to slot of
  // this class.
  connect(_medEventListener, SIGNAL(medEventSignal(const MEDCALC::MedEvent*)),
    this, SLOT(processMedEvent(const MEDCALC::MedEvent*)));
  // >>> WARN:
  // Note that this class must be mocked (Q_OBJECT + moc file
  // generated from header file) so that to be able to connect a
  // signal to a slot of this class.

  // -------------------------------------------------------------
  // Customize the treeview rendering the datamodel with specific
  // action for the popup menu
  this->getDataTreeView()->clearActions();
  _actionIds.display    = this->getDataTreeView()->addAction(tr("VISUALIZE_SCALAR_MAP"));
  _actionIds.useInTui   = this->getDataTreeView()->addAction(tr("USE_IN_CONSOLE"));
  _actionIds.exportToPv = this->getDataTreeView()->addAction(tr("EXPORT_TO_PARAVIS"));
  _actionIds.save       = this->getDataTreeView()->addAction(tr("SAVE_AS_MED"));
  _actionIds.remove     = this->getDataTreeView()->addAction(tr("REMOVE_FROM_WORKSPACE"));

  // -------------------------------------------------------------
  // Initialize the python console. Note that this must be done at
  // last because the setup will try to initiate a connection to the
  // event listener.
  _consoleDriver = new XmedConsoleDriver(salomeModule);
  _consoleDriver->setup();
}

WorkspaceController::~WorkspaceController() {
  STDLOG("WorkspaceController::~WorkspaceController()");
  MEDEventListener_i::release();
}

/**
 * This creates the GUI actions for driving the Workspace. The
 * WorkspaceController creates itself this actions and implements the
 * connected slots.
 */
void WorkspaceController::createActions() {
  // QWidget* dsk = _salomeModule->getApp()->desktop(); // todo: unused
  // SUIT_ResourceMgr* resMgr = _salomeModule->getApp()->resourceMgr(); // todo: unused
  int toolbarId = _salomeModule->createTool("Workspace", "WorkspaceToolbar");

  QString label   = tr("LAB_SAVE_WORKSPACE");
  QString tooltip = tr("TIP_SAVE_WORKSPACE");
  QString icon    = tr("ICO_WORKSPACE_SAVE");
  int actionId = _salomeModule->createStandardAction(label,this,SLOT(OnSaveWorkspace()),icon,tooltip);
  _salomeModule->createTool(actionId, toolbarId);

  label   = tr("LAB_CLEAN_WORKSPACE");
  tooltip = tr("TIP_CLEAN_WORKSPACE");
  icon    = tr("ICO_WORKSPACE_CLEAN");
  actionId = _salomeModule->createStandardAction(label,this,SLOT(OnCleanWorkspace()),icon,tooltip);
  _salomeModule->createTool(actionId, toolbarId);
}

/*!
 * Implementation of the slot processItemList inherited from TreeGuiManager
 */
void WorkspaceController::processItemList(QStringList itemNameIdList, int actionId) {
  if ( actionId == _actionIds.display ) {
    STDLOG("WorkspaceController::processItemList: display");
    this->_viewItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.useInTui ) {
    STDLOG("WorkspaceController::processItemList: use");
    this->_importItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.exportToPv ) {
    STDLOG("WorkspaceController::processItemList: export");
    this->_exportItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.save ) {
    STDLOG("WorkspaceController::processItemList: save");
    this->_saveItemList(itemNameIdList);
  }
  else if ( actionId == _actionIds.remove ) {
    STDLOG("WorkspaceController::processItemList: remove");
    this->_removeItemList(itemNameIdList);
  }
  else {
    STDLOG("WorkspaceController::processItemList: ERR : action unknown ");
  }
}

/*!
 * This function import in the console all the fields associated to
 * the model items of the specified list. "Import a fields" means
 * "define a field proxy variable in the python context to manipulate
 * the real field in the database".
 */
void WorkspaceController::_importItemList(QStringList itemNameIdList) {
  LOG("WorkspaceController: signal received : display item list "<<itemNameIdList);
  QStringList::const_iterator it;
  for (it = itemNameIdList.constBegin(); it != itemNameIdList.constEnd(); ++it) {
    QString itemNameId = *it;
    this->_importItem(itemNameId);
  }
}

/*!
 * This function is the unit function used to import field in the
 * console (see _importItemList).
 */
void WorkspaceController::_importItem(QString itemNameId) {
  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    LOG("No data model associated to this tree view");
    return;
  }

  // We can request the dataModel to obtain the dataObject associated
  // to this item (iteNameId is a TreeView id, Qt stuff only).
  XmedDataObject* dataObject =
    (XmedDataObject*)dataModel->getDataObject(QS2S(itemNameId));

  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }

  // Then, we can request this data object to obtain the associated
  // FieldHandler.
  MEDCALC::FieldHandler* fieldHandler = dataObject->getFieldHandler();
  STDLOG("Field: mesh="<<fieldHandler->meshname<<" name="<<fieldHandler->fieldname);

  // Finally, we can import the field
  bool askForOptions = true;
  _importFieldIntoConsole(fieldHandler, askForOptions);
}

/**
 * This function import the specified field into the tui console. This
 * means to define a field proxy variable in the python context to
 * manipulate the field. We can raise a gui to specify some import
 * options or simply specify the alias (i.e. the name of the python
 * variable).
 */
void WorkspaceController::_importFieldIntoConsole(MEDCALC::FieldHandler* fieldHandler,
              bool askForOptions,
              const char* alias)
{
  STDLOG("alias="<<alias);

  // By default, the alias is the name of the field
  QString*effectiveAlias;
  if ( alias == NULL ) {
    effectiveAlias = new QString(fieldHandler->fieldname);
  }
  else {
    effectiveAlias = new QString(alias);
  }

  // We can propose to the user to specify some additional
  // information concerning what must be imported.
  //
  // In this version, we just ask the alias the field will be
  // manipulated with. The default alias is the field name. This alias
  // should be asked to the user to get a short name to manipulate.
  if ( askForOptions ) {
    DlgAlias dialog;
    dialog.setAlias(*effectiveAlias);
    int choice = dialog.exec();
    if ( choice == QDialog::Rejected ) {
      // The user decides to cancel the operation
      return;
    }
    *effectiveAlias = dialog.getAlias();
  }

  //
  // Then, the list of python commands can be written and executed to
  // define the field in the console
  //
  QStringList commands;
  commands+=QString("%1=medcalc.newFieldProxy(fieldHandlerId=%2)")
    .arg(*effectiveAlias)
    .arg(fieldHandler->id);

  _consoleDriver->exec(commands);
}

/*!
 * This function is a Qt slot connected to the signal medEventSignal
 * emitted from the MEDEventListener. It processes events coming from
 * the python console.
 */
void WorkspaceController::processMedEvent(const MEDCALC::MedEvent* event) {
  STDLOG("WorkspaceController::processMedEvent");
  STDLOG("dataId  :"<<event->dataId);

  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }

  if ( event->type == MEDCALC::EVENT_UPDATE_FIELD ) {
    std::cout << "WorkspaceController::processMedEvent[MEDCALC::EVENT_UPDATE_FIELD]: Not implemented yet";
  }
  else if ( event->type == MEDCALC::EVENT_PUT_IN_WORKSPACE ) {
    STDLOG("add new field");
    MEDCALC::FieldHandler* fieldHandler =
      MEDFactoryClient::getDataManager()->getFieldHandler(event->dataId);

    XmedDataObject* dataObject = (XmedDataObject*)dataModel->newDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    this->getDataTreeModel()->addData(dataObject);
  }
  else if ( event->type == MEDCALC::EVENT_REMOVE_FROM_WORKSPACE ) {
    STDLOG("remove field");
    std::map<std::string, DataObject*>::iterator itr = dataModel->begin();
    for ( ; itr != dataModel->end(); ++itr) {
      XmedDataObject* obj = dynamic_cast<XmedDataObject*>(itr->second);
      if (obj->getFieldHandler()->id == event->dataId) {
        std::string itemNameId = obj->getNameId();
        this->getDataTreeModel()->removeData(obj);
        dataModel->removeDataObject(itemNameId);
        return;
      }
    }
  }
  else if ( event->type == MEDCALC::EVENT_CLEAN_WORKSPACE ) {
    STDLOG("clean workspace");
    std::map<std::string, DataObject*>::iterator itr = dataModel->begin();
    for ( ; itr != dataModel->end(); ++itr) {
      XmedDataObject* obj = dynamic_cast<XmedDataObject*>(itr->second);
      std::string itemNameId = obj->getNameId();
      this->getDataTreeModel()->removeData(obj);
      dataModel->removeDataObject(itemNameId);
    }
  }
  else if ( event->type == MEDCALC::EVENT_ADD_DATASOURCE ) {
    emit workspaceSignal(event); // forward to DatasourceController
  }
  else if ( event->type == MEDCALC::EVENT_ADD_PRESENTATION ) {
    emit workspaceSignal(event); // forward to PresentationController
  }
  else if ( event->type == MEDCALC::EVENT_REMOVE_PRESENTATION ) {
    emit workspaceSignal(event); // forward to PresentationController
  }
  else if ( event->type == MEDCALC::EVENT_VISIBILITY_CHANGED ) {
    emit workspaceSignal(event); // forward to PresentationController
  }
  else if ( event->type == MEDCALC::EVENT_MODIFY_PRESENTATION ) {
      emit workspaceSignal(event); // forward to PresentationController
  }
  else if ( event->type == MEDCALC::EVENT_CHANGE_UNDERLYING_MESH
            || event->type == MEDCALC::EVENT_INTERPOLATE_FIELD ) {
    int fieldId = event->dataId;
    MEDCALC::FieldHandler* fieldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
    XmedDataObject* dataObject = new XmedDataObject();
    dataObject->setFieldHandler(*fieldHandler);
    std::cout << "IMPORT object in workspace: " << dataObject->toString() << std::endl;
    STDLOG("IMPORT object in workspace:\n"<<dataObject->toString());
    // _GBO_ QUESTION: tag automatically the object as a peristant object ??
    // We first add the data object to the internal data model
    dataModel->addDataObject(dataObject);
    // Then we request the tree view to consider this new object
    this->getDataTreeModel()->addData(dataObject);

    // Workaround to visualize the result
    MEDCouplingFieldDouble* fieldDouble = MEDDataManager_i::getInstance()->getFieldDouble(fieldHandler);
    std::string filename = std::tmpnam(NULL);
    WriteField(filename.c_str(), fieldDouble, true);

    QStringList commands;
    commands += QString("source_id = medcalc.LoadDataSource('%1')").arg(filename.c_str());
    commands += QString("source_id");
    commands += QString("mesh_id = medcalc.GetFirstMeshFromDataSource(source_id)");
    commands += QString("mesh_id");
    commands += QString("field_id = medcalc.GetFirstFieldFromMesh(mesh_id)");
    commands += QString("field_id");
    commands += QString("presentation_id = medcalc.MakeScalarMap(accessField(field_id), viewMode=MEDCALC.VIEW_MODE_REPLACE)");
    commands += QString("presentation_id");
    _consoleDriver->exec(commands);
  }
  else if ( event->type == MEDCALC::EVENT_PLAY_TEST ) {
    emit workspaceSignal(event); // forward to TestController
  }
  else if ( event->type == MEDCALC::EVENT_QUIT_SALOME ) {
    emit workspaceSignal(event); // forward to TestController
  }
  else if ( event->type == MEDCALC::EVENT_ERROR ) {
      std::string msg(event->msg);
      QMessageBox::warning(_salomeModule->getApp()->desktop(), "Error", QString::fromStdString(msg));
  }
  else {
    STDLOG("WorkspaceController::processMedEvent(): Unhandled event!!!");
  }
}

/*!
 * This function save a list of fields in a med file. The med file
 * name is requested to the user using a file chooser dialog box
 */
void WorkspaceController::_saveItemList(QStringList itemNameIdList) {
  XmedDataProcessor* dataProcessor = new XmedDataProcessor(this->getDataModel());
  dataProcessor->process(itemNameIdList);
  MEDCALC::FieldIdList_var fieldIdList = dataProcessor->getResultingFieldIdList();
  delete dataProcessor;

  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));
  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               tr("SAVE_SELECTED_FIELDS"),
                                               false);

  if ( filename.isEmpty() ) return;

  MEDFactoryClient::getDataManager()->saveFields(QCHARSTAR(filename), fieldIdList);
}

/*!
 * This function remove the selected item from workspace.
 */
void WorkspaceController::_removeItemList(QStringList itemNameIdList) {
  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    LOG("No data model associated to this tree view");
    return;
  }

  // __GBO__: In this version, we consider only the first field in the selection
  QString itemNameId = itemNameIdList[0];

  // We can request the dataModel to obtain the dataObject associated
  // to this item (iteNameId is a TreeView id, Qt stuff only).
  XmedDataObject* dataObject =
    (XmedDataObject*)dataModel->getDataObject(QS2S(itemNameId));

  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }

  // Then, we can request this data object to obtain the associated
  // FieldHandler.
  MEDCALC::FieldHandler* fieldHandler = dataObject->getFieldHandler();
  STDLOG("Field: mesh="<<fieldHandler->meshname<<" name="<<fieldHandler->fieldname);

  // Remove the field variable from console
  QStringList commands;
  commands+=QString("removeFromWorkspace(accessField(%1))").arg(fieldHandler->id);
  _consoleDriver->exec(commands);

  // Finally, we can remove the field from tree data model and tree view
  this->getDataTreeModel()->removeData(dataObject);
  dataModel->removeDataObject(QS2S(itemNameId));
}

/**
 * This function export the list of specified field item to PARAVIS
 * module. This consists in create a med file gathering the selected
 * items, then to import this file in PARAVIS, and finally to create a
 * scalar map of the first item to start the job.
 */
void WorkspaceController::_exportItemList(QStringList itemNameIdList) {
  XmedDataProcessor* dataProcessor = new XmedDataProcessor(this->getDataModel());
  dataProcessor->process(itemNameIdList);
  MEDCALC::FieldIdList_var fieldIdList = dataProcessor->getResultingFieldIdList();
  delete dataProcessor;

  // _GBO_ We use a temporary file to proceed with this export to
  // paravis. I'm sure it could be better in a futur version or when I
  // will get a better understanding of paravis API.
  const char* tmpfilename = "/tmp/medcalc_export2paravis.med";
  MEDFactoryClient::getDataManager()->saveFields(tmpfilename, fieldIdList);

  // We import the whole file but create a scalar map for the first
  // selected field only (it's just an export to continue the job in
  // paravis)
  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }
  QString itemNameId = itemNameIdList[0];
  XmedDataObject* dataObject = (XmedDataObject*)dataModel->getDataObject(QS2S(itemNameId));
  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }
  QStringList commands;
  /*
  MEDCALC::FieldHandler* fieldHandler = dataObject->getFieldHandler(); // todo: unused
  commands+=QString("from xmed.driver_pvis import pvis_scalarmap");
  commands+=QString("pvis_scalarmap('%1','%2','%3',%4,%5)")
    .arg(tmpfilename)
    .arg(QString(fieldHandler->meshname))
    .arg(QString(fieldHandler->fieldname))
    .arg(fieldHandler->type)
    .arg(fieldHandler->iteration);
  */
  commands += "print('Not implemented yet')";
  _consoleDriver->exec(commands);

}

/*!
 * This function sends a request to the SALOME data visualisation
 * (module VISU or PARAVIS) for displaying a scalar map of the fields
 * associated to the model items in the specified list.
 *
 */
void WorkspaceController::_viewItemList(QStringList itemNameIdList) {

  // __GBO__: In this version, we consider only the first field in the selection
  QString itemNameId = itemNameIdList[0];

  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    LOG("No data model associated to this tree view");
    return;
  }

  // We can request the dataModel to obtain the dataObject associated
  // to this item (iteNameId is a TreeView id, Qt stuff only).
  XmedDataObject* dataObject =
    (XmedDataObject*)dataModel->getDataObject(QS2S(itemNameId));
  if ( dataObject == NULL ) {
    LOG("WorkspaceController: WARN! No data object associated to the item "<<itemNameId);
    return;
  }

  // Then, we can request this data object to obtain the associated
  // FieldHandler.
  //MEDCALC::FieldHandler* fieldHandler = dataObject->getFieldHandler(); // todo: unused

  // And finally, we can create the set of medcalc instructions to
  // generate the scalar map on this field.
  QStringList commands;
  //commands+=QString("view(accessField(%1))").arg(fieldHandler->id);
  commands += "print('Not implemented yet')";
  _consoleDriver->exec(commands);
}

/**
 * This slot can process the event coming from the
 * DatasourceController. The connection between the datasource signal
 * and this slot is realized by the main class MEDModule.
 */
void WorkspaceController::processDatasourceEvent(const DatasourceEvent* event) {
  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }

  // >>>
  // __GBO__ To know what to do we should test the type, because the
  // object could be a mesh, a timeseries or a single field. We test
  // here the case of a single field. Moreover, there could have
  // options such that "change the underlying mesh".
  // <<<

  XmedDataObject* dataObject = event->objectdata;

  if ( event->eventtype == DatasourceEvent::EVENT_IMPORT_OBJECT ) {
    std::cout << "IMPORT object in workspace: " << dataObject->toString() << std::endl;
    STDLOG("IMPORT object in workspace:\n"<<dataObject->toString());
    // _GBO_ QUESTION: tag automatically the object as a peristant object ??
    // We first add the data object to the internal data model
    dataModel->addDataObject(dataObject);
    // Then we request the tree view to consider this new object
    this->getDataTreeModel()->addData(dataObject);
  }
  else if ( event->eventtype == DatasourceEvent::EVENT_USE_OBJECT ) {
    STDLOG("USE object in workspace:\n"<<dataObject->toString());
    // We first add the data object to the internal data model
    dataModel->addDataObject(dataObject);
    // Then we request the tree view to consider this new object
    this->getDataTreeModel()->addData(dataObject);

    // We define a proxy for this object in the tui console.
    STDLOG("Define a proxy variable in the console with name : "<<QCHARSTAR(event->objectalias));
    bool askForOptions = false;
    _importFieldIntoConsole(dataObject->getFieldHandler(),
          askForOptions,
          QCHARSTAR(event->objectalias));
  }
  else if ( event->eventtype == DatasourceEvent::EVENT_ADD_DATASOURCE ) {
    QStringList commands;
    commands += QString("source_id = medcalc.LoadDataSource('%1')").arg(event->objectalias);
    commands += QString("source_id");
    _consoleDriver->exec(commands);

    // Create a default presentation when loading a file
    MEDCALC::MedEvent* evt = new MEDCALC::MedEvent();
    evt->type = MEDCALC::EVENT_ADD_PRESENTATION;
    evt->dataId = -1;
    emit workspaceSignal(evt); // forward to PresentationController
  }
  else if ( event->eventtype == DatasourceEvent::EVENT_ADD_IMAGE_AS_DATASOURCE ) {
    QStringList commands;
    commands += QString("source_id = medcalc.LoadImageAsDataSource('%1')").arg(event->objectalias);
    commands += QString("source_id");
    _consoleDriver->exec(commands);
  }
  else {
    STDLOG("The event "<<event->eventtype<<" is not implemented yet");
  }
}

void
WorkspaceController::processProcessingEvent(const ProcessingEvent* event)
{
  XmedDataModel* dataModel = (XmedDataModel*)this->getDataModel();
  if ( dataModel == NULL ) {
    STDLOG("No data model associated to this tree view");
    return;
  }

  int fieldId = event->fieldId;
  int meshId = event->meshId;

  if ( event->eventtype == ProcessingEvent::EVENT_CHANGE_UNDERLYING_MESH ) {
    QStringList commands;
    commands += QString("result_id = medcalc.ChangeUnderlyingMesh(fieldId=%1,meshId=%2)").arg(fieldId).arg(meshId);
    commands += QString("result_id");
    _consoleDriver->exec(commands);
  }
  else if ( event->eventtype == ProcessingEvent::EVENT_INTERPOLATE_FIELD ) {
    MEDCALC::InterpolationParameters params = event->interpParams;
    QString method = QString(params.method);
    QString nature = QString(params.nature);
    QString intersectionType = QString(params.intersectionType);

    QStringList commands;
    commands += QString("result_id = medcalc.InterpolateField(fieldId=%1,meshId=%2,precision=%3,defaultValue=%4,reverse=%5,method='%6',nature='%7',intersectionType='%8')").arg(fieldId).arg(meshId).arg(params.precision).arg(params.defaultValue).arg(params.reverse).arg(method).arg(nature).arg(intersectionType);
    commands += QString("result_id");
    _consoleDriver->exec(commands);
  }
}

void WorkspaceController::OnSaveWorkspace() {

  // Dialog to get the filename where the workspace must be saved into
  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));

  QString filename = SUIT_FileDlg::getFileName(_salomeModule->getApp()->desktop(),
                                               "",
                                               filter,
                                               tr("SAVE_WORKSPACE_DATA"),
                                               false);

  if ( filename.isEmpty() ) return;

  STDLOG("OnWorkspaceSave: save the workspace in the file " << QCHARSTAR(filename));
  QStringList commands;
  commands+=QString("saveWorkspace('%1')").arg(filename);
  _consoleDriver->exec(commands);
}

#include <QMessageBox>
void WorkspaceController::OnCleanWorkspace() {
  // Remove field from console
  QStringList commands;
  commands += QString("cleanWorkspace()");
  _consoleDriver->exec(commands);
}
