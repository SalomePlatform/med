// Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#ifndef _MED_MODULE_HXX_
#define _MED_MODULE_HXX_

#include <Python.h>
#include "MEDCALCGUI.hxx"

#include <SalomeApp_Module.h>
#include <SALOME_AppStudyEditor.hxx>

#include "WorkspaceController.hxx"
#include "XmedDataModel.hxx"
#include "DatasourceController.hxx"
#include "PresentationController.hxx"
#include "ProcessingController.hxx"
#include "TestController.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MEDPresentationManager)

#include <QModelIndex>

class SalomeApp_Application;
class PVViewer_GUIElements;
class MEDPresentationManager_i;
class LightApp_Displayer;

/*!
 * This class defines the gui of the MED module.
 */
class MEDCALCGUI_EXPORT MEDModule: public SalomeApp_Module
{
  Q_OBJECT

  // ===========================================================
  // This part implements the standard interface of a SALOME gui
  // ===========================================================

public:
  MEDModule();
  ~MEDModule();

  static MED_ORB::MED_Gen_var engine();

  virtual void initialize(CAM_Application* app);
  virtual QString engineIOR() const;

  virtual QString iconName() const;

  virtual void windows(QMap<int, int>& theMap) const;
  virtual void viewManagers(QStringList& theList) const;

  virtual void createPreferences();

  int createStandardAction(const QString& label,
                           QObject* slotobject,
                           const char* slotmember,
                           const QString& iconName,
                           const QString& tooltip=QString(),
                           int actionid=-1);
  void addActionInPopupMenu(int actionId,
                            const QString& menus="",
                            const QString& rule="client='ObjectBrowser'");

//  MEDCALC::ViewModeType getSelectedViewMode() const;
//  MEDCALC::ColorMapType getSelectedColorMap() const;
//  MEDCALC::ScalarBarRangeType getSelectedScalarBarRange() const;

  inline SALOME_AppStudyEditor* getStudyEditor() { return _studyEditor; }
  void requestSALOMETermination() const;

  bool itemClickGeneric(std::string& name, std::string& type, int& presId) const;

  int getIntParamFromStudyEditor(SALOMEDS::SObject_var obj, const char* name);

  double getCurrentAnimationTimestamp();

  virtual LightApp_Displayer* displayer() override;

  void updateVisibilityState(const bool all=true, const QStringList& entries = QStringList());

  void visibilityStateUpdateOff();
  void visibilityStateUpdateOn();

signals:
  void presentationSelected(int presId, const QString& presType, const QString& presName);

public slots:
  virtual bool activateModule(SUIT_Study* theStudy);
  virtual bool deactivateModule(SUIT_Study* theStudy);
  virtual void onDblClick(const QModelIndex& index);
  virtual void onClick(const QModelIndex& index);

private:
  void createModuleWidgets();
  void createModuleActions();
  void initToolbars();

  static void init();

private:
  SALOME_AppStudyEditor* _studyEditor;
  DatasourceController* _datasourceController;
  WorkspaceController* _workspaceController;
  XmedDataModel* _xmedDataModel;
  PresentationController* _presentationController;
  ProcessingController* _processingController;

  PVViewer_GUIElements*   _pvGuiElements;

  static MED_ORB::MED_Gen_var _MED_engine;
  
  LightApp_Displayer*          _displayer;

  bool _enableVisibilityStateUpdate;

#ifdef MED_HAS_QTTESTING
  TestController * _testController;
#endif
};

#endif
