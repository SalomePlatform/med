// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDGUIDataBaseDockWidget.hxx"

#include "CAM_Application.h"
#include "SUIT_ResourceMgr.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QGridLayout>

MEDGUIDataBaseDockWidget::MEDGUIDataBaseDockWidget(CAM_Application *app, QWidget * parent):QDockWidget(parent),_app(app)
{
  setWindowTitle("MEDUtils DataBase");
  setObjectName("MEDUtils DataBase");
  setMinimumWidth(270);
  setMinimumHeight(300);
  QWidget *content = new QWidget;
  setWidget(content);
  QGridLayout *gridLayout1 = new QGridLayout(content);
  _tree_widget=new QTreeWidget(content);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QTreeWidgetItem *item1 = new QTreeWidgetItem;
  item1->setText(0, QString::fromUtf8("Name"));
  _tree_widget->setHeaderItem(item1);
  sizePolicy.setHeightForWidth(_tree_widget->sizePolicy().hasHeightForWidth());
  _tree_widget->setSizePolicy(sizePolicy);
  _tree_widget->setMinimumSize(QSize(0, 0));
  _tree_widget->setSizeIncrement(QSize(0, 0));
  _tree_widget->header()->setCascadingSectionResizes(true);
  gridLayout1->addWidget(_tree_widget,0,0,1,1);
  //
  QPixmap pixmap;
  SUIT_ResourceMgr* mgr=_app->resourceMgr();
  pixmap=mgr->loadPixmap("FIELDS","med_field.png");
  _fields=new QTreeWidgetItem((QTreeWidget*)0,QStringList(QString("Fields")));
  _fields->setIcon(0,QIcon(pixmap));
  _tree_widget->addTopLevelItem(_fields);
  pixmap=mgr->loadPixmap("FIELDS","med_mesh.png");
  _meshes=new QTreeWidgetItem((QTreeWidget*)0,QStringList(QString("Meshes")));
  _meshes->setIcon(0,QIcon(pixmap));
  _tree_widget->addTopLevelItem(_meshes);
}

void MEDGUIDataBaseDockWidget::appendFieldLT(const std::vector<ParaMEDMEM::MEDCalculatorBrowserLiteStruct>& lts)
{
}
