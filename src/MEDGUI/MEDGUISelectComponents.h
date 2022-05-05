// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : MEDGUISelectComponents.h
//  Module : MED
//
#ifndef __MEDGUISELECTCOMPONENTS_H__
#define __MEDGUISELECTCOMPONENTS_H__

#include <QDialog>

#include "MEDCalculatorBrowserLiteStruct.hxx"


namespace Ui
{
  class MEDGUISelectComponents;
}

//  This class is implemented to provide a dialog box for selecting fields components and/or setting components names
//  The basic graphic part was made with QtDesigner4 and is in MedGUI_FileContentDial.ui file
//  It use a MedGUI_LiteStruct object to get components information
//  If one field is selected (call made from a step or a field), the box will only display components from this field
//  If all fields are selected (call made on MED file), the box will display all the components for each field

class MEDGUISelectComponents : public QDialog
{
  Q_OBJECT
  public:
  MEDGUISelectComponents(ParaMEDMEM::MEDCalculatorBrowserLiteStruct&, QWidget *parent = 0, int fieldInd = -1);//  Default constructor
  ~MEDGUISelectComponents();//  Destructor

protected:
  void changeEvent(QEvent *e);

public slots:
  void fillSelection();//  Method to fill the Lightstruct according to the selection and modifications made by user

private:
  Ui::MEDGUISelectComponents *ui;
  ParaMEDMEM::MEDCalculatorBrowserLiteStruct* ls;
  int fieldInd;
};

#endif
