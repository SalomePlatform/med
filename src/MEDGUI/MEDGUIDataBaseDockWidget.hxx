// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef __MEDGUIDATABASEDOCKWIDGET_HXX__
#define __MEDGUIDATABASEDOCKWIDGET_HXX__

#include <QDockWidget>

#include "MEDCalculatorBrowserLiteStruct.hxx"

class QTreeWidget;
class QTreeWidgetItem;
class CAM_Application;

class MEDGUIDataBaseDockWidget : public QDockWidget
{
 public:
  MEDGUIDataBaseDockWidget(CAM_Application *, QWidget * parent);
  void appendFieldLT(const std::vector<ParaMEDMEM::MEDCalculatorBrowserLiteStruct>& lts);
protected:
  CAM_Application *_app;
  QTreeWidget *_tree_widget;
  QTreeWidgetItem *_fields;
  QTreeWidgetItem *_meshes;
protected:
  std::vector<ParaMEDMEM::MEDCalculatorBrowserLiteStruct> _field_lts;
};

#endif
