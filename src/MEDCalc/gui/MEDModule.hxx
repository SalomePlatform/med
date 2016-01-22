// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
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

#include "MEDCALCGUI.hxx"

#include <SalomeApp_Module.h>

#include "WorkspaceController.hxx"
#include "XmedDataModel.hxx"
#include "DatasourceController.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED_Gen)

class SalomeApp_Application;

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

  virtual void                    initialize( CAM_Application* app );
  virtual QString                 engineIOR() const;

  virtual QString                 iconName() const;

  virtual void                    windows( QMap<int, int>& theMap ) const;
  virtual void                    viewManagers( QStringList& theList ) const;

  int createStandardAction(const QString& label,
                           QObject * slotobject,
                           const char* slotmember,
                           const QString& iconName,
                           const QString& tooltip=QString());
  void addActionInPopupMenu(int actionId,const QString& menus="",const QString& rule="client='ObjectBrowser'");

public slots:
  virtual bool                    activateModule( SUIT_Study* theStudy );
  virtual bool                    deactivateModule( SUIT_Study* theStudy );

private:
  void createModuleWidgets();
  void createModuleActions();
  static void init();

private:
  DatasourceController * _datasourceController;
  WorkspaceController *  _workspaceController;
  XmedDataModel *        _xmedDataModel;
  static MED_ORB::MED_Gen_var myEngine;
};

#endif