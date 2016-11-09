// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

#include "MEDModule.hxx"
#include "QtHelper.hxx"
#include <MEDCalcConstants.hxx>

#include "SALOME_LifeCycleCORBA.hxx"
#include "QtxPopupMgr.h"

#include <LightApp_Preferences.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_DataBrowser.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataObject.h>
#include <SalomeApp_DataModel.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)
#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>

#ifndef DISABLE_PVVIEWER
#include <PVViewer_ViewModel.h>
#include <PVViewer_GUIElements.h>
#endif

#include "MEDFactoryClient.hxx"
#include "MEDPresentationManager_i.hxx"

#include <QTimer>
#include <sstream>

#include <pqAnimationManager.h>
#include <pqPVApplicationCore.h>

//! The only instance of the reference to engine
MED_ORB::MED_Gen_var MEDModule::_MED_engine;

MEDModule::MEDModule() :
  SalomeApp_Module("MED"), _studyEditor(0),
  _datasourceController(0), _workspaceController(0), _presentationController(0),
  _processingController(0), _pvGuiElements(0)
{
  // Note also that we can't use the getApp() function here because
  // the initialize(...) function has not been called yet.

  init(); // internal initialization
}

MEDModule::~MEDModule()
{
  if (_studyEditor)
    delete _studyEditor;
  if (_datasourceController)
    delete _datasourceController;
  //if (_workspaceController)
  //  delete _workspaceController;
  if (_presentationController)
    delete _presentationController;
  if (_processingController)
    delete _processingController;
}

MED_ORB::MED_Gen_var
MEDModule::engine()
{
  init(); // initialize engine, if necessary
  return _MED_engine;
}

void
MEDModule::init()
{
  // initialize MED module engine (load, if necessary)
  if ( CORBA::is_nil( _MED_engine ) ) {
    Engines::EngineComponent_var comp =
      SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "MED" );
    if (CORBA::is_nil( comp ))
        STDLOG("Could not FindOrLoad_Component MED");
    _MED_engine = MED_ORB::MED_Gen::_narrow( comp );
    if (CORBA::is_nil( _MED_engine ))
        STDLOG("Could not narrow MED engine");
  }
}

//void MEDModule::onEventLoopStarted()
//{
//  if(!getApp()->isMainEventLoopStarted())
//    {
//      QTimer::singleShot(100, this, SLOT(onEventLoopStarted()));
//      return;
//    }
//}

void
MEDModule::initialize( CAM_Application* app )
{
  // call the parent implementation
  SalomeApp_Module::initialize( app );

  getApp()->objectBrowser()->setAutoOpenLevel(5);

  if (app && app->desktop()) {
    connect((QObject*) (getApp()->objectBrowser()->treeView()), SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(onDblClick(const QModelIndex&)));
    connect((QObject*) (getApp()->objectBrowser()->treeView()), SIGNAL(clicked(const QModelIndex&)),
                this, SLOT(onClick(const QModelIndex&)));
  }

  // The following initializes the GUI widget and associated actions
  this->createModuleWidgets();
  this->createModuleActions();

  // Now that the workspace controller is created, ParaView core application has normally been started,
  // and hidden GUI elements have been created.  We can fire the VCR toolbar activation:
  initToolbars();
}

QString
MEDModule::engineIOR() const
{
  init(); // initialize engine, if necessary
  CORBA::String_var anIOR = getApp()->orb()->object_to_string( _MED_engine.in() );
  return QString( anIOR.in() );
}

QString
MEDModule::iconName() const
{
  return tr("ICO_MED_SMALL");
}

void
MEDModule::windows( QMap<int, int>& theMap ) const
{
  // want Object browser, in the left area
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser,
                 Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
  // want Python console, in the bottom area
  theMap.insert( SalomeApp_Application::WT_PyConsole,
                 Qt::BottomDockWidgetArea );
#endif
}

void
MEDModule::viewManagers( QStringList& list ) const
{
#ifndef DISABLE_PVVIEWER
  list.append( PVViewer_Viewer::Type() );
#endif
}

void
MEDModule::createPreferences()
{
  int genTab = addPreference(tr("PREF_TAB_GENERAL"));

  int themeGroup = addPreference(tr("PREF_THEME_GROUP"), genTab);
  setPreferenceProperty(themeGroup, "columns", 2);
  int icons = addPreference(tr("PREF_ICONS"), themeGroup, LightApp_Preferences::Selector, "MEDCalc", "icons" );
  QStringList iconsThemes;
  iconsThemes.append(tr("PREF_ICON_THEME_MODERN"));
  iconsThemes.append(tr("PREF_ICON_THEME_CLASSIC"));
  QList<QVariant> indices;
  indices.append(0);
  indices.append(1);
  setPreferenceProperty(icons, "strings", iconsThemes);
  setPreferenceProperty(icons, "indexes", indices);
}

bool
MEDModule::activateModule( SUIT_Study* theStudy )
{
  if ( CORBA::is_nil( _MED_engine ) )
    return false;

  // call parent implementation
  bool bOk = SalomeApp_Module::activateModule( theStudy );
  if (!bOk)
    return false;

  // show own menus
  setMenuShown( true );
  // show own toolbars
  setToolShown( true );

  //this->createStudyComponent(theStudy);
  _workspaceController->showDockWidgets(true);
  _presentationController->showDockWidgets(true);
  //this->setDockLayout(StandardApp_Module::DOCKLAYOUT_LEFT_VLARGE);

  // Mark the start of the main event loop - important for test playback:
//  QObject::connect(getApp(), SIGNAL(activated(SUIT_Application *)), this, SLOT(onEventLoopStarted(SUIT_Application *)));
//  QTimer::singleShot(0, this, SLOT(onEventLoopStarted()));

  // return the activation status
  return bOk;
}

bool
MEDModule::deactivateModule( SUIT_Study* theStudy )
{
 // Clean up engine:
  STDLOG("MEDModule::deactivateModule(): cleaning up engine side.");
  _MED_engine->cleanUp();
  MEDFactoryClient::getFactory()->getPresentationManager()->cleanUp();
  MEDFactoryClient::getFactory()->getDataManager()->cleanUp();

  _workspaceController->showDockWidgets(false);
  _presentationController->showDockWidgets(false);
  //this->unsetDockLayout();

  // hide own menus
  setMenuShown( false );
  // hide own toolbars
  setToolShown( false );

  // call parent implementation and return the activation status
  return SalomeApp_Module::deactivateModule( theStudy );
}

//
// =====================================================================
// This part add custom widgets (a dockwidget containing a tree view
// in this example) and add special actions in the toolbox of the
// module.
// =====================================================================
//

/*!
 * This function implements the interface StandardApp_Module. It
 * creates the widgets specific for this module, in particular the
 * workspace widget and the dataspace widget.
 */
void
MEDModule::createModuleWidgets() {
  _studyEditor = new SALOME_AppStudyEditor(getApp());
  _datasourceController = new DatasourceController(this);
  _workspaceController = new WorkspaceController(this);
  _xmedDataModel  = new XmedDataModel();
  _workspaceController->setDataModel(_xmedDataModel);
  _presentationController = new PresentationController(this);
  // ABN: ultimately console driver should be owned by module: everyone needs it, not only WorkspaceController
  _presentationController->setConsoleDriver(_workspaceController->getConsoleDriver());
  _processingController = new ProcessingController(this);
#ifdef MED_HAS_QTTESTING
  _testController = new TestController(this);
#endif

  connect(_datasourceController, SIGNAL(datasourceSignal(const DatasourceEvent*)),
    _workspaceController, SLOT(processDatasourceEvent(const DatasourceEvent*)));

  connect(_presentationController, SIGNAL(presentationSignal(const PresentationEvent*)),
    _presentationController, SLOT(processPresentationEvent(const PresentationEvent*)));

  connect(_processingController, SIGNAL(processingSignal(const ProcessingEvent*)),
    _workspaceController, SLOT(processProcessingEvent(const ProcessingEvent*)));

  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDCALC::MedEvent*)),
    _datasourceController, SLOT(processWorkspaceEvent(const MEDCALC::MedEvent*)));

  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDCALC::MedEvent*)),
    _presentationController, SLOT(processWorkspaceEvent(const MEDCALC::MedEvent*)));
}

void
MEDModule::initToolbars()
{
  // VCR and Time toolbars:
  SUIT_Desktop* dsk = getApp()->desktop();
  _pvGuiElements = PVViewer_GUIElements::GetInstance(dsk);

  _pvGuiElements->getVCRToolbar();  // make sure VCR toolbar is built
  _pvGuiElements->setToolBarVisible(false);
  _pvGuiElements->setVCRTimeToolBarVisible(true);

  // Emit signal in order to make sure that animation scene is set - same trick as in PARAVIS module activation
  QMetaObject::invokeMethod( pqPVApplicationCore::instance()->animationManager(),
                             "activeSceneChanged",
                             Q_ARG( pqAnimationScene*, pqPVApplicationCore::instance()->animationManager()->getActiveScene() ) );

#ifdef MED_HAS_QTTESTING
  connect(_workspaceController, SIGNAL(workspaceSignal(const MEDCALC::MedEvent*)),
    _testController, SLOT(processWorkspaceEvent(const MEDCALC::MedEvent*)));
#endif
}

void
MEDModule::createModuleActions() {
  _datasourceController->createActions();
  _workspaceController->createActions();
  _presentationController->createActions();
  _processingController->createActions();
#ifdef MED_HAS_QTTESTING
  _testController->createActions();
#endif
}

int
MEDModule::createStandardAction(const QString& label,
                                QObject* slotobject,
                                const char* slotmember,
                                const QString& iconName,
                                const QString& tooltip)
{
  SUIT_Desktop* dsk = getApp()->desktop();
  SUIT_ResourceMgr* resMgr = getApp()->resourceMgr();

  // If the tooltip is not defined, we choose instead the label text.
  QString effToolTip(tooltip);
  if ( effToolTip.isEmpty() )
    effToolTip = label;

  QIcon ico;
  if (iconName.isEmpty())
    ico = QIcon();
  else
    ico = QIcon(resMgr->loadPixmap("MED", iconName));

  QAction* action = createAction(-1,
                                 label,
                                 ico,
                                 label,
                                 effToolTip,
                                 0,
                                 dsk,
                                 false,
                                 slotobject,
                                 slotmember
                                 );
  return actionId(action);
}

void
MEDModule::addActionInPopupMenu(int actionId,const QString& menus,const QString& rule)
{
  // _GBO_ for a fine customization of the rule (for example with a
  // test on the type of the selected object), see the LIGHT module:
  // implement "LightApp_Selection*    createSelection() const;"
  int parentId = -1;
  QtxPopupMgr* mgr = this->popupMgr();
  this->action( actionId )->setIconVisibleInMenu(true);
  if (! menus.isEmpty())
    mgr->insert ( this->action( actionId ), menus, parentId, 0 );
  else
    mgr->insert ( this->action( actionId ), parentId, 0 );
  mgr->setRule( this->action( actionId ), rule, QtxPopupMgr::VisibleRule );
}

//MEDCALC::ViewModeType
//MEDModule::getSelectedViewMode() const
//{
//  return _presentationController->getSelectedViewMode();
//}
//
//MEDCALC::ColorMapType
//MEDModule::getSelectedColorMap() const
//{
//  return _presentationController->getSelectedColorMap();
//}
//
//MEDCALC::ScalarBarRangeType
//MEDModule::getSelectedScalarBarRange() const
//{
//  return _presentationController->getSelectedScalarBarRange();
//}


/**
 * Returns presentation name, type and ID from the currently selected presentation in the object
 * browser.
 */
bool
MEDModule::itemClickGeneric(std::string & name, std::string & type, int & presId) const
{
  DataObjectList dol = getApp()->objectBrowser()->getSelected();
  if (dol.isEmpty())
    return false;
  SalomeApp_DataObject* item = dynamic_cast<SalomeApp_DataObject*>(dol[0]);
  if (!item)
    return false;
  SalomeApp_DataModel *model = dynamic_cast<SalomeApp_DataModel*>(dataModel());
  if (!model)
    return false;

  if (item->componentDataType().toStdString() != "MED")
    return false;
  _PTR(SObject) obj = item->object();
  _PTR(GenericAttribute) anAttribute;

  if (! obj->FindAttribute(anAttribute, "AttributeName"))
    return false;
  _PTR(AttributeName) attrName(anAttribute);
  name = attrName->Value();

  if (! obj->FindAttribute(anAttribute, "AttributeParameter"))
    return false;
  _PTR(AttributeParameter) attrParam(anAttribute);
  if (! attrParam->IsSet(IS_PRESENTATION, PT_BOOLEAN)
      || ! attrParam->GetBool(IS_PRESENTATION)) { // Not a presentation
      return false;
  }
//  if (!attrParam->IsSet(FIELD_ID, PT_INTEGER))
//    return false;
//  fieldId = attrParam->GetInt(FIELD_ID);
  if (!attrParam->IsSet(PRESENTATION_ID, PT_INTEGER))
      return false;
  presId = attrParam->GetInt(PRESENTATION_ID);
  if (!attrParam->IsSet(PRESENTATION_TYPE, PT_STRING))
    return false;
  type = attrParam->GetString(PRESENTATION_TYPE);
  return true;
}

void
MEDModule::onClick(const QModelIndex & index)
{
  int presId;
  std::string name, type;
  if (!itemClickGeneric(name, type, presId))
    {
      // Not a presentation - clear widget:
      emit presentationSelected(-1, QString(""), QString(""));
      return;
    }

//  STDLOG("Presentation selection");
//  std::ostringstream oss;
//  oss << fieldId << " / " << presId;
//  STDLOG("    - Field id / pres id:   " + oss.str());
//  STDLOG("    - Presentation type: " + type);
//  STDLOG("    - Presentation name: " + name);

  emit presentationSelected(presId, QString::fromStdString(type), QString::fromStdString(name) );  // caught by PresentationController
}

void
MEDModule::onDblClick(const QModelIndex& index)
{
  int presId;
  std::string name, type;
  if (!itemClickGeneric(name, type, presId))
    return;

//  STDLOG("Presentation double click");
//  STDLOG("  Presentation infos:");
//  STDLOG("    - Component:         " + item->componentDataType().toStdString());
//  STDLOG("    - Item entry:        " + item->entry().toStdString());
//  STDLOG("    - Item name:         " + item->name().toStdString());
//  std::ostringstream oss;
//  oss << fieldId;
//  STDLOG("    - Field id:          " + oss.str());
//  STDLOG("    - Presentation name: " + name);
}

void
MEDModule::requestSALOMETermination() const
{
  STDLOG("Requesting SALOME termination!!");
  SUIT_Session::session()->closeSession( SUIT_Session::DONT_SAVE, 1 );  // killServers = True
}


//bool MEDModule::hasMainEventLoopStarted() const
//{
//  return _eventLoopStarted;
//}

int
MEDModule::getIntParamFromStudyEditor(SALOMEDS::SObject_var obj, const char* name)
{
  if (obj->_is_nil())
    return -1;

  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeParameter_var aParam;
  if ( obj->FindAttribute(anAttr,"AttributeParameter") ) {
    aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
    if (aParam->IsSet(name, PT_INTEGER))
      return aParam->GetInt(name);
  }
  return -1;
}
