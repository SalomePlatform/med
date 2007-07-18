//  MED MEDGUI : MED component GUI implemetation
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MedGUI.cxx
//  Module : MED

#include "MedGUI.h"

// SALOME Includes
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "utilities.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>
#include <SalomeApp_Tools.h>

#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <SUIT_FileDlg.h>
#include <SUIT_ResourceMgr.h>

#include <CAM_Application.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DataModel.h>
#include <SalomeApp_Study.h>
#include <SALOMEDSClient_Study.hxx>
#include <SALOMEDSClient_SObject.hxx>

#include <LightApp_SelectionMgr.h>

#include <QAction>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>


#include <MedGUI_Selection.h>

// QT Includes
//#include <qinputdialog.h>

//VRV: porting on Qt 3.0.5
//#if QT_VERSION >= 0x030005
//#include <qlineedit.h>
//#endif
//VRV: porting on Qt 3.0.5

using namespace std;

static CORBA::ORB_var   _orb;

//=============================================================================
/*!
 *
 */
//=============================================================================
MedGUI::MedGUI() :
  SalomeApp_Module( "MED" )
{
}

//=============================================================================
/*!
 *
 */
//=============================================================================
/* 
void MedGUI::createPopupItem( const int id,
                              const QString& clients,
                              const QString& types,
                              const QString& theRule,
			      const int pId )
{
  int parentId = pId;
  if( pId!=-1 )
    parentId = popupMgr()->actionId( action( pId ) );

  if( !popupMgr()->contains( popupMgr()->actionId( action( id ) ) ) )
    popupMgr()->insert( action( id ), parentId, 0 );

  QChar lc = QtxPopupMgr::Selection::defEquality();
  QString rule = "(%1)";
  if( !types.isEmpty() )
    rule += " and (%2) and (%3)";

  rule = rule.arg( QString( "client in {%1}" ).arg( clients ) );

  if( !types.isEmpty() )
  {
    rule = rule.arg( QString( "%1>0" ).arg( QtxPopupMgr::Selection::defSelCountParam() ) );
    rule = rule.arg( QString( "%1type in {%2}" ).arg( lc ).arg( types ) );
  }
  rule += theRule;
  popupMgr()->setRule( action( id ), rule, true );
}
*/

void MedGUI::createMedAction( const int id, const QString& po_id, const QString& icon_id )
{
  QWidget* parent = application()->desktop();
  SUIT_ResourceMgr* mgr = application()->resourceMgr();

  QPixmap pix;
  QIcon icon;
  if( !icon_id.isEmpty() )
    pix = mgr->loadPixmap( "MED", tr( (const char*)icon_id.toLatin1() ) );
//   else
//     pix = mgr->loadPixmap( "MED", tr( QString( "ICO_" )+po_id ) );

  if ( !pix.isNull() )
    icon = QIcon( pix );

  createAction( id, 
		tr( (const char*)("TOP_" + po_id).toLatin1() ),
		icon,
		tr( (const char*)("MEN_" + po_id).toLatin1() ),
		tr( (const char*)("STB_" + po_id).toLatin1() ),
		0,
		parent,
		false,
		this,
		SLOT( onGUIEvent() ) );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  //QWidget* parent = application()->desktop();

  createMedAction( 931, "MESHSEL" );
  createMedAction( 932, "FIELDSEL" );
  createMedAction( 933, "EXPLORE" );
  createMedAction( 934, "DUMPMESH" );
  createMedAction( 935, "DUMPSUBMESH" );
//   createMedAction( 8031, "POPUPTEST" );
//   createMedAction( 9002, "ERASE" );
//   createMedAction( 9003, "DISPLAY" );
  createMedAction( 4031, "MESHSEL", "ICO_TB_MESHSEL" );
  createMedAction( 4032, "FIELDSEL", "ICO_TB_FIELDSEL" );
  createMedAction( 4033, "EXPLORE", "ICO_TB_EXPLORE" );

  int MedId = createMenu( tr( "MED" ), -1, 50, 10 );
  createMenu( separator(), MedId, 10 );
  createMenu( 931, MedId, 11 );
  createMenu( 932, MedId, 11 );
  createMenu( 933, MedId, 11 );
  createMenu( 934, MedId, 11 );
  createMenu( 935, MedId, 11 );

  int medTb = createTool( tr( "TB_MED" ) );
  createTool( 4031, medTb );
  createTool( 4032, medTb );
  createTool( 4033, medTb );

//   QString OB = "'ObjectBrowser'",
//           View = QString("'%1'").arg( "VTKViewer" /* SVTK_Viewer::Type()*/ );

//   createPopupItem( 8031, View, "", "" );
//   createPopupItem( 9002, OB, "", "" );
//   createPopupItem( 903,  OB, "", "" );
}

void MedGUI::contextMenuPopup( const QString& client, QMenu* menu, QString& /*title*/ )
{  
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  if( app )
  {
    MedGUI_Selection sel( client, app->selectionMgr() );
    popupMgr()->setMenu( menu );
    popupMgr()->setSelection( &sel );
    popupMgr()->updateMenu();
  }
}

QString MedGUI::engineIOR() const
{
  QString anIOR( "" );
  SALOME_MED::MED_Gen_ptr aMedGen = InitMedGen();
  if ( !CORBA::is_nil( aMedGen) )
  {
    CORBA::String_var objStr = getApp()->orb()->object_to_string( aMedGen );
    anIOR = QString( objStr.in() );
  }
  return anIOR;
}

void MedGUI::windows( QMap<int, int>& mappa ) const
{
  mappa.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  mappa.insert( SalomeApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
}


//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::onGUIEvent()
{
  const QAction* obj = qobject_cast<QAction *>(sender());
  if ( !obj );
    return;
  int id = actionId( obj );
  if ( id != -1 )
    OnGUIEvent( id );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::EmitSignalCloseAllDialogs()
{
  emit SignalCloseAllDialogs();
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::deactivateModule( SUIT_Study* study )
{
  setMenuShown( false );
  setToolShown( false );

  disconnect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
	     this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  EmitSignalCloseAllDialogs();

  return SalomeApp_Module::deactivateModule( study );
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::activateModule( SUIT_Study* study )
{
  bool res = SalomeApp_Module::activateModule( study );

  setMenuShown( true );
  setToolShown( true );

  connect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
	  this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );
  return res;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::onWindowActivated( SUIT_ViewWindow* )
{
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnGUIEvent (int theCommandID)
{
  setOrb();

  SalomeApp_Study* myActiveStudy = dynamic_cast< SalomeApp_Study* >( application()->activeStudy() );
  if( !myActiveStudy )
    return false;

  _PTR(Study) aStudy = myActiveStudy->studyDS();
  //SALOME_NamingService* myNameService = parent->getNameService();

  QString file;
  QStringList filtersList ;

  filtersList.append( tr("MED_MEN_IMPORT_MED") );
  filtersList.append( tr("MED_MEN_ALL_FILES") ) ;

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  if( !app )
    return false;

  switch (theCommandID)
    {
    case 4031:
    case 9031:
    case 931:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->studyName();
	bool ok=FALSE;
// 	int myStudyId = myActiveStudy->id();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen();

	// Selection du Fichier
	file = SUIT_FileDlg::getFileName(application()->desktop(),
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);

        // Selection du Maillage
	if (!file.isEmpty() )
	  {
	    SCRUTE((const char*)file.toLatin1());
	    QString meshName;
	    meshName = QInputDialog::getText( application()->desktop(),
					      tr("MED_INF_MESHNAME"),
					      QString::null,
					      QLineEdit::Normal,
					      QString::null,
					      &ok );
	    if ( ok && !meshName.isEmpty() )
	      {
		  try
		    {
		      medgen->readMeshInFile( (const char*)file.toLatin1(),
					      (const char*)myStudyName.toLatin1(),
					      (const char*)meshName.toLatin1() );
		      if (myActiveStudy->studyDS()->GetProperties()->IsLocked()) {
			SUIT_MessageBox::warning ( application()->desktop(),
					           tr("WRN_WARNING"),
					           tr("WRN_STUDY_LOCKED") );
			//QObject::tr("BUT_OK")); by default
		      }
		    }
		  catch (const SALOME::SALOME_Exception & S_ex)
		    {
		      SalomeApp_Tools::QtCatchCorbaException(S_ex);
		    }
		}
	        updateObjBrowser();
	  }
	break;
      }
    case 4032:
    case 9032:
    case 932:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->studyName();
	bool ok=FALSE;
// 	int myStudyId = myActiveStudy->id();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen();

	// Selection du Fichier
	file = SUIT_FileDlg::getFileName(application()->desktop(),
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);

        // Selection du Maillage
	if (!file.isEmpty() )
	  {
	    SCRUTE((const char*)file.toLatin1());
	    QString fieldName;
	    fieldName = QInputDialog::getText( application()->desktop(),
					       tr("MED_INF_FIELDNAME"),
					       QString::null,
					       QLineEdit::Normal,
					       QString::null,
					       &ok );
	    if ( ok && !fieldName.isEmpty())
	      {
		try
		  {
		    medgen->readFieldInFile( (const char*)file.toLatin1(),
					     (const char*)myStudyName.toLatin1(),
					     (const char*)fieldName.toLatin1(),
					     -1,
					     -1 );
		    if (myActiveStudy->studyDS()->GetProperties()->IsLocked()) {
		      SUIT_MessageBox::warning ( application()->desktop(),
					         tr("WRN_WARNING"),
					         tr("WRN_STUDY_LOCKED") );
		      //tr("BUT_OK")); by default
		    }
		  }
		catch (const SALOME::SALOME_Exception & S_ex)
		  {
		    SalomeApp_Tools::QtCatchCorbaException(S_ex);
		  }
		updateObjBrowser ();
	      }
	  }
	break;
      }
    case 4033:
    case 933:
      {
	MESSAGE("command " << theCommandID << " activated");

	QString myStudyName = myActiveStudy->studyName();
// 	int myStudyId = myActiveStudy->id();

	// load MED engine
	SALOME_MED::MED_Gen_ptr medgen = InitMedGen();

	// Selection du Fichier
	file = SUIT_FileDlg::getFileName(application()->desktop(),
					"",
					filtersList,
					tr("MED_MEN_IMPORT"),
					true);
	if (!file.isEmpty() )
	  {
	    SCRUTE((const char*)file.toLatin1());
	    try
	      {
//		medgen->readStructFile(file.latin1(),myStudyName);
		medgen->readStructFileWithFieldType((const char*)file.toLatin1(),
                                                    (const char*)myStudyName.toLatin1());

		MESSAGE("Ouais on est la !!!!");

		if (myActiveStudy->studyDS()->GetProperties()->IsLocked()) {

		  MESSAGE("Ouais on est la 1 !!!!");

		  SUIT_MessageBox::warning (application()->desktop(),
                                            tr("WRN_WARNING"),
                                            tr("WRN_STUDY_LOCKED") );
		  //QObject::tr("BUT_OK")); by default
		}
	      }
	    catch (const SALOME::SALOME_Exception & S_ex)
	      {
		MESSAGE("Ouais on est la 2 !!!!");

		SalomeApp_Tools::QtCatchCorbaException(S_ex);
	      }

	    MESSAGE("Ouais on est la 3 !!!!");

	    updateObjBrowser ();
	  }
	break;
      }

    case 934:
      {
	//Handle(SMESH_TypeFilter) aMeshFilter = new SMESH_TypeFilter( MESH );

	SALOME_ListIO list;
	SalomeApp_Application* app = getApp();
	LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : NULL;
	if( mgr )
	  mgr->selectedObjects( list );
	//Sel->AddFilter(aMeshFilter) ;

	int nbSel = list.Extent();
	if ( nbSel == 1 )
	  {
	    //	    SMESH::SMESH_Mesh_var aM;
	    SALOME_MED::MESH_var aMesh;
	    Handle(SALOME_InteractiveObject) IObject = list.First();
	    if ( IObject->hasEntry() )
	      {
		_PTR(SObject) aMorSM = aStudy->FindObjectID( IObject->getEntry() );
		if ( aMorSM )
		  {
		    _PTR(GenericAttribute) anAttr;
		    _PTR(AttributeIOR)     anIOR;
		    if (aMorSM->FindAttribute(anAttr, "AttributeIOR"))
		      {
			anIOR = anAttr;
			aMesh = SALOME_MED::MESH::_narrow( _orb->string_to_object(anIOR->Value().c_str()) );
			if ( aMesh->_is_nil() )
			  {
			    //  			    aM = SMESH::SMESH_Mesh::_narrow(_orb->string_to_object(anIOR->Value()));
			    //  			    if ( aM->_is_nil() )
			    //  			      {
			    //  				QAD_MessageBox::warn1
			    //  				  ( QAD_Application::getDesktop(),
			    //  				    tr ("MED_WRN_WARNING"),
			    //  				    tr ("MED_INF_NOTIMPL"),
			    //  				    tr ("MED_BUT_OK") );
			    //  				break;
			    //  			      }
			    //  			    aMesh = aM->GetMEDMesh();
			    if ( aMesh->_is_nil() )
			      {
				SUIT_MessageBox::warning
				  ( application()->desktop(),
				    tr ("MED_WRN_WARNING"),
				    tr ("MED_INF_NOTIMPL") );
				//tr ("MED_BUT_OK") ); by default
				break;
			      }
  			  }
  			DumpMesh( aMesh );
			//Sel->ClearFilters() ;
		      }
		    else
		      {
			SUIT_MessageBox::warning
			  ( application()->desktop(),
			    tr ("MED_WRN_WARNING"),
			    tr ("MED_INF_NOIOR") );
			//tr ("MED_BUT_OK") ); by default
			break;
		      }
		  }
	      }
	  }
	break;
      }

    case 935:
      {
	//Handle(SMESH_TypeFilter) aSubMeshFilter = new SMESH_TypeFilter( SUBMESH );

	SALOME_ListIO list;
	SalomeApp_Application* app = getApp();
	LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : NULL;
	if( mgr )
	  mgr->selectedObjects( list );

	//Sel->AddFilter(aSubMeshFilter) ;

	int nbSel = list.Extent();
	if ( nbSel == 1 )
	  {
	    //	    SMESH::SMESH_subMesh_var aSubM;
	    SALOME_MED::FAMILY_var aFam;
	    Handle(SALOME_InteractiveObject) IObject = list.First();
	    if ( IObject->hasEntry() )
	      {
		_PTR(SObject) aMorSM = aStudy->FindObjectID( IObject->getEntry() );
		if ( aMorSM )
		  {
		    _PTR(GenericAttribute) anAttr;
		    _PTR(AttributeIOR)     anIOR;
		    if (aMorSM->FindAttribute(anAttr, "AttributeIOR"))
		      {
			anIOR = anAttr;
			//			aSubM = SMESH::SMESH_subMesh::_narrow( _orb->string_to_object(anIOR->Value()) );
			//  			if ( aSubM->_is_nil() )
			//  			  {
			//  			    aFam=SALOME_MED::FAMILY::_narrow( _orb->string_to_object(anIOR->Value()));
			//  			    if ( aFam->_is_nil() )
			//  			      {
			//  				QAD_MessageBox::warn1
			//  				  ( QAD_Application::getDesktop(),
			//  				    tr ("MED_WRN_WARNING"),
			//  				    tr ("MED_INF_NOTIMPL"),
			//  				    tr ("MED_BUT_OK") );
			//  				break;
			//  			      }
			//  			    DumpSubMesh( aFam );
			//    }
			//  			else
			//  			  {
			//  			    DumpSubMesh( aSubM );
			//Sel->ClearFilters() ;
			//       }
		      }
		    else
		      {
			SUIT_MessageBox::warning
			  ( application()->desktop(),
			    tr ("MED_WRN_WARNING"),
			    tr ("MED_INFNOIOR") );
			//  tr ("MED_BUT_OK") ); by default
			break;
		      }
		  }
	      }
	  }
	break;
      }
    }

  app->updateActions(); //SRN: To update a Save button in the toolbar

  return true;
}


//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnMousePress (QMouseEvent* pe ,
			   SUIT_ViewWindow* wnd )
{
  MESSAGE("MedGUI::OnMousePress");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnMouseMove (QMouseEvent* pe ,
			  SUIT_ViewWindow* wnd )
{
  //   MESSAGE("MedGUI::OnMouseMouve");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::OnKeyPress (QKeyEvent* pe,
			 SUIT_ViewWindow* wnd)
{
  MESSAGE("MedGUI::OnKeyPress");
  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
/*bool MedGUI::SetSettings ()
{
  MESSAGE("MedGUI::SetSettings");
  return true;
}*/

//=============================================================================
/*!
 *
 */
//=============================================================================
/*bool MedGUI::CustomPopup ( QPopupMenu* popup,
			   const QString & theContext,
			   const QString & theParent,
			   const QString & theObject )
{
  MESSAGE("MedGUI::CustomPopup");
  popup->clear();
  return true;
}*/

//=============================================================================
/*!
 *
 */
//=============================================================================
/*bool MedGUI::ActiveStudyChanged()
{
  return true;
}*/

//=============================================================================
/*!
 *
 */
//=============================================================================
/*void MedGUI::DefinePopup( QString & theContext, QString & theParent, QString & theObject )
{
  theObject = "";
  theContext = "";
}
*/
//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::DumpMesh( SALOME_MED::MESH_var MEDMesh)
{

  if ( MEDMesh->_is_nil() )
    {
      return false;
    }

  //SALOME_MED::MESH_var MEDMesh = aMesh->GetMEDMesh();
  string name = MEDMesh->getName();
  SCRUTE(name);

  int dim = MEDMesh->getMeshDimension();
  SCRUTE(dim);
  int dim2 = MEDMesh->getSpaceDimension();
  SCRUTE(dim2);

  int k = MEDMesh->getNumberOfNodes() ;
  SCRUTE(k);
  SALOME_MED::double_array_var coords = MEDMesh->getCoordinates( SALOME_MED::MED_FULL_INTERLACE );
  int i = 0;
  int lu = 0;
  while (lu < k ) {
    if (dim2==3)
      {
    	MESSAGE ( " Coordinates  X = " << coords[i] << " Y = " << coords[i+1] << " Z = " << coords[i+2] );
    	i = i + 3; // Only for triangles
      }
    else
      {
    	MESSAGE ( " Coordinates  X = " << coords[i] << " Y = " << coords[i+1] );
    	i = i + 2;
      }
    lu=lu+1;
  }

  int nbfam=MEDMesh->getNumberOfFamilies(SALOME_MED::MED_NODE);
  SCRUTE(nbfam);
  SALOME_MED::Family_array_var Families=MEDMesh->getFamilies(SALOME_MED::MED_NODE) ;

  for (k=0;  k < nbfam; k++) {
    SCRUTE(k);
    string nomFam=Families[k]->getName();
    SCRUTE(nomFam);
    int identfam=Families[k]->getIdentifier();
    SCRUTE(identfam);
    int nbelemnts=Families[k]->getNumberOfElements(SALOME_MED::MED_NONE);
    SCRUTE(nbelemnts);
    SALOME_MED::long_array_var tabnoeuds=Families[k]->getNumber(SALOME_MED::MED_NONE);
    for (int l=0;l<(int)tabnoeuds->length();l++)
      SCRUTE(tabnoeuds[l]);
  }

  //     int famIdent = 1;
  //     SALOME_MED::FAMILY_ptr Family=MEDMesh->getFamily(SALOME_MED::MED_NODE,1) ;
  //     MESSAGE("ici");
  //     string nomFam=Family->getName();
  //     SCRUTE(nomFam);
  //     int identfam=Family->getIdentifier();
  //     SCRUTE(identfam);
  //     SALOME_MED::long_array_var tabnoeuds=Family->getNumber(SALOME_MED::MED_NONE);
  //     for (int l=0;l<tabnoeuds->length();l++)
  //       SCRUTE(tabnoeuds[l]);

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
//  bool MedGUI::DumpSubMesh( SMESH::SMESH_subMesh_ptr aSubMesh )
//  {
//    if ( aSubMesh->_is_nil() )
//      return false;

//    SALOME_MED::FAMILY_var Fam = aSubMesh->GetFamily();
//    if ( Fam->_is_nil() )
//      return false;

//    SALOME_MED::long_array_var tabnoeuds=Fam->getNumber(SALOME_MED::MED_NONE);
//    for (int l=0;l<tabnoeuds->length();l++)
//      SCRUTE(tabnoeuds[l]);

//    return true;
//  }
//=============================================================================
/*!
 *
 */
//=============================================================================
bool MedGUI::DumpSubMesh( SALOME_MED::FAMILY_var Fam )
{

  if ( Fam->_is_nil() )
    return false;

  SALOME_MED::long_array_var tabnoeuds=Fam->getNumber(SALOME_MED::MED_NONE);
  for (int l=0;l<(int)tabnoeuds->length();l++)
    SCRUTE(tabnoeuds[l]);

  return true;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
SALOME_MED::MED_Gen_ptr MedGUI::InitMedGen() const
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  Engines::Component_var comp =
    SALOME_LifeCycleCORBA(app->namingService()).FindOrLoad_Component( "FactoryServer", "MED" );

  MESSAGE("_________________________________________");
  SALOME_MED::MED_Gen_var clr = SALOME_MED::MED_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr._retn();
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void MedGUI::setOrb()
{
  try {
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
    _orb = init( 0 , 0 );
  } catch (...) {
    INFOS("internal error : orb not found");
    _orb = 0;
  }
  ASSERT(! CORBA::is_nil(_orb));
}

extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new MedGUI();
  }
}
