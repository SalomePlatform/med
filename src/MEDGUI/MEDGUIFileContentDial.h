// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  MED MEDGUI : MED component GUI implementation
//  File   : MEDGUIFileContentDial.h
//  Module : MED
//
#ifndef __MEDGUIFILECONTENTDIAL_H__
#define __MEDGUIFILECONTENTDIAL_H__

#include "MEDCalculatorBrowserLiteStruct.hxx"

#include <QDialog>
#include <QTreeWidgetItem>

class MEDGUIDataBaseDockWidget;

namespace Ui
{
  class MEDGUIFileContentDial;
}

class MEDGUIFileContentDial : public QDialog
{
  Q_OBJECT
    public:
  MEDGUIFileContentDial(MEDGUIDataBaseDockWidget*, QWidget* = 0);
  ~MEDGUIFileContentDial();
  
 protected:
  void changeEvent(QEvent *e);
  void selFromFile(QTreeWidgetItem*, QString);//  Method to set all elements of a MedGUI_LiteStruct to (un)selected according to a string (such as "all", "none", ...)
  void selFromField(QTreeWidgetItem*, QString);//  Method to set all elements of a MedGUI_Field to (un)selected according to a string (such as "all", "none", ...)
  void selFromStep(QTreeWidgetItem*,QString);//  Method to set all elements of a MedGUI_Field when selection is made from a MedGUI_Step to (un)selected according to a string (such as "all", "none", ...)
  
public slots:
  void openFile();//  Create a new MedGUI_LiteStruct from a file and add content to QTreeWidgets
  void meshesStateChange(QTreeWidgetItem*, int);//  Change the select statement of one or more meshes, the item represents a mesh or a file
  void fieldsStateChange(QTreeWidgetItem*, int);//  Change the select statement of one or more fields, the item represents a mesh or a field or a step
  void fieldsStateChanges();
  void sendSelectionToDB();
  void unselectAll();//  Unselec all fields and meshes
  void overfield(QTreeWidgetItem*,int);//  Show in red the corresponding meshes from a field or a step or a field hoovered by mouse
  void selCompoPopup(bool);//  Show the dialog for selecting components and changing components names
  void selStepsPopup(bool);//  Show the QLineEdit for selecting steps
  
private:
  Ui::MEDGUIFileContentDial *ui;
  MEDGUIDataBaseDockWidget *_db;
  std::vector<ParaMEDMEM::MEDCalculatorBrowserLiteStruct> litestructs;//  Liste of files (MEdGUI_LiteStruct)
  std::vector<QTreeWidgetItem*> coloredMeshes;//  Liste of meshes currently colored in red
};

#endif // MEDGUI_FILECONTENTDIAL_HXX
