// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
//  File   : MEDGUIFileContentDial.cxx
//  Module : MED
#include "MEDGUIFileContentDial.h"
#include "ui_MEDGUIFileContentDial.h"

#include <QString>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMainWindow>

#include "MEDCalculatorBrowserLiteStruct.hxx"
#include "MEDCalculatorBrowserField.hxx"
#include "MEDCalculatorBrowserStep.hxx"
#include "MEDCalculatorBrowserMesh.hxx"

#include "MEDGUISelectComponents.h"
#include "MEDGUIDataBaseDockWidget.hxx"

#include <sstream>
#include <iostream>
#include <algorithm>

//  Default constructor
//  Set Qt specific var (ui)
//  Connect buttons to actions, close statement is already connect in the .ui file
//  Set QTreeWidgets parameters : header labels, selection mode
//  Connect QTreeWidget itemClicked signal to meshes and fieldsStateChange
//  Put mouse tracking on, so this class received mouse positions information when mouse is above treewidget
//  Add contextual menu to treewidget's items with the call to selectCompoPopup and selStepPopup
//  Connect those actions to corresponding signals
MEDGUIFileContentDial::MEDGUIFileContentDial(MEDGUIDataBaseDockWidget* db, QWidget* parent):
  QDialog(parent, 0), ui(new Ui::MEDGUIFileContentDial), _db(db)
{
  setModal( false );
  setSizeGripEnabled( true );
  setAttribute( Qt::WA_DeleteOnClose );

  QWidget* w = new QWidget( this );
  QHBoxLayout* l = new QHBoxLayout( this );
  l->setMargin( 0 );
  l->addWidget( w );
  ui->setupUi(w);

  connect(ui->importMedFileButton, SIGNAL(clicked()), this, SLOT(openFile()));
  connect(ui->addSelectionButton, SIGNAL(clicked()), this, SLOT(sendSelectionToDB()));
  connect(ui->unselectAllButton, SIGNAL(clicked()), this, SLOT(unselectAll()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

  ui->treeWidgetFields->setHeaderLabel("Available Field");
  ui->treeWidgetFields->setSelectionMode(QAbstractItemView::MultiSelection);
  ui->treeWidgetMeshes->setHeaderLabel("Available Meshes");
  ui->treeWidgetMeshes->setSelectionMode(QAbstractItemView::MultiSelection);

  connect(ui->treeWidgetMeshes, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(meshesStateChange(QTreeWidgetItem*, int)));
  connect(ui->treeWidgetFields, SIGNAL(itemSelectionChanged()), this, SLOT(fieldsStateChanges()));
  //connect(ui->treeWidgetFields, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(fieldsStateChange(QTreeWidgetItem*, int)));

  ui->treeWidgetFields->setMouseTracking(true);
  connect(ui->treeWidgetFields,SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(overfield(QTreeWidgetItem*,int)));

  ui->treeWidgetFields->setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction* selcompo = new QAction("Select components",ui->treeWidgetFields);
  QAction* selsteps = new QAction("Select steps",ui->treeWidgetFields);
  ui->treeWidgetFields->addAction(selcompo);
  ui->treeWidgetFields->addAction(selsteps);

  connect(selcompo,SIGNAL(triggered(bool)),this,SLOT(selCompoPopup(bool)));
  connect(selsteps,SIGNAL(triggered(bool)),this,SLOT(selStepsPopup(bool)));

}

//  Destructor
MEDGUIFileContentDial::~MEDGUIFileContentDial()
{
  delete ui;
}

//  Qt specific
void MEDGUIFileContentDial::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

//  OpenFile
//  Create a new MEDGUILiteStruct with a file as parameter.
//  First the file is selected using a QFileDialog
//  Then, after checking if the name already exist
//  - if yes, try to add "_#" where # begin at 2
//  - if not, continue
//  Create a new MEDGUILiteStruct with the complete file name and the simplified file name as parameters
//  Add items to the two QTreeWidget get information from the new MEDGUILiteStruct
void MEDGUIFileContentDial::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(this,tr("Open Med File"),QDir::homePath(),tr("Med File (*.med)"));
  if(fileName != QString("")){
    QString realname = fileName.split("/").last();

    int i = 2;
    std::string name = realname.toStdString();
    while(std::find(litestructs.begin(),litestructs.end(),name) != litestructs.end())
    {
      std::ostringstream oss;
      oss << realname.toStdString() << "_" << i++ ;
      name = oss.str();
    }

    ParaMEDMEM::MEDCalculatorBrowserLiteStruct ls(fileName.toStdString().c_str());
    litestructs.push_back(ls);

    QTreeWidgetItem* root = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
    for (unsigned int i = 0; i < ls.getNumberOfFields(); i += 1)
    {
      QTreeWidgetItem* fieldname = new QTreeWidgetItem(root,QStringList(QString(tr(ls.getFieldName(i).c_str()))));
      ParaMEDMEM::MEDCalculatorBrowserField field = ls.getField(i);
      for (unsigned int j = 0; j < field.getStepsSize(); j += 1)
      {
        std::ostringstream input;
        input<<field.getSteps()[j].getTimeStep()<<" ( "<<field.getSteps()[j].getTimeValue()<<" )";
        new QTreeWidgetItem(fieldname,QStringList(QString(tr(input.str().c_str()))));
      }
    }
    ui->treeWidgetFields->addTopLevelItem(root);

    QTreeWidgetItem* root_2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
    for (unsigned int i = 0; i < ls.getNumberOfMeshes(); i += 1)
    {
      new QTreeWidgetItem(root_2,QStringList(QString(tr(ls.getMeshName(i).c_str()))));
    }
    ui->treeWidgetMeshes->addTopLevelItem(root_2);
  }
}

//  Change the select flag of one or more meshes
//  If the QTreeWidgetItem correspond to file (it has no QTreeWidgetItem above itself), it set the state of all meshes of this file according to it current state (if it's selected, all meshes will be selected)
//  If the QTreeWidgetItem is a mesh, juste the mesh state is changed. If the mesh is unselected, its father is checked, if it has no more selection inside, it's unselected, else, it stays selected
void MEDGUIFileContentDial::meshesStateChange(QTreeWidgetItem* qtwi, int col)
{
  QTreeWidgetItem *father = qtwi->parent();
  if(!father){
    if (qtwi->isSelected())
    {
      litestructs[ui->treeWidgetMeshes->indexOfTopLevelItem(qtwi)].selectAllMeshes();
      for ( int i = 0; i < qtwi->childCount(); i += 1)
      {
        qtwi->child(i)->setSelected(true);
      }
    }else{
      litestructs[ui->treeWidgetMeshes->indexOfTopLevelItem(qtwi)].unselectAllMeshes();
      for ( int i = 0; i < qtwi->childCount(); i += 1)
      {
        qtwi->child(i)->setSelected(false);
      }
    }
  }else{
    int lsInd =ui->treeWidgetMeshes->indexOfTopLevelItem(father);
    if (qtwi->isSelected())
    {
      litestructs[lsInd].selectMesh(litestructs[lsInd].getMeshName(father->indexOfChild(qtwi)));
      father->setSelected(true);
    }else{
      litestructs[lsInd].unselectMesh(litestructs[lsInd].getMeshName(father->indexOfChild(qtwi)));
      bool stillselection=false;
      for ( int i = 0; i < father->childCount(); i += 1)
      {
        if(father->child(i)->isSelected()){
          stillselection = true;
          break;
        }
      }
      father->setSelected(stillselection);
    }
  }
}

void MEDGUIFileContentDial::fieldsStateChanges()
{
  disconnect(ui->treeWidgetFields, SIGNAL(itemSelectionChanged()), this, SLOT(fieldsStateChanges()));
  unselectAll();
  QList<QTreeWidgetItem *> l=ui->treeWidgetFields->selectedItems();
  for(QList<QTreeWidgetItem *>::iterator it=l.begin();it!=l.end();it++)
    fieldsStateChange(*it,0);
  connect(ui->treeWidgetFields, SIGNAL(itemSelectionChanged()), this, SLOT(fieldsStateChanges()));
}

//  Change the select flag of one or more fields
//  If the QTreeWidgetItem correspond to file (it has no QTreeWidgetItem above itself), it set the state of all fields, and all steps of this file according to it current state (if it's selected, all elements will be selected)
//  If the QTreeWidgetItem is a field (it has no QTreeWidgetItem above is "father"), juste the field and its steps state is changed. If the field is unselected , his father is checked, if it has no more selection inside, it's unselected, else, it stays selected.
//  If the QTreeWidgetItem is a step, juste the step state is changed. If the step is unselected , his father is checked and his grandfather are checked, if they have no more selection inside, they're unselected, else, they stays selected.
void MEDGUIFileContentDial::fieldsStateChange(QTreeWidgetItem* qtwi, int col)
{
  QTreeWidgetItem *father = qtwi->parent();
  if(!father)
    {//  File
      if(qtwi->isSelected())
        {
          litestructs[ui->treeWidgetFields->indexOfTopLevelItem(qtwi)].selectAllFields();
          for ( int i = 0; i < qtwi->childCount(); i += 1)
            {
              qtwi->child(i)->setSelected(true);
              for ( int j = 0; j < qtwi->child(i)->childCount(); j += 1)
                qtwi->child(i)->child(j)->setSelected(true);
            }
        }
      else
        {
          litestructs[ui->treeWidgetFields->indexOfTopLevelItem(qtwi)].unselectAllFields();
          for ( int i = 0; i < qtwi->childCount(); i += 1)
            {
              qtwi->child(i)->setSelected(false);
              for ( int j = 0; j < qtwi->child(i)->childCount(); j += 1)
                qtwi->child(i)->child(j)->setSelected(false);
            }
        }
    }
  else if(!father->parent())
    {//  Field
      int lsInd = ui->treeWidgetFields->indexOfTopLevelItem(father);
      int fieldInd = father->indexOfChild(qtwi);
      std::string fieldName = litestructs[lsInd].getFieldName(fieldInd);
      if(qtwi->isSelected())
        {
          litestructs[lsInd].selectField(fieldName);
          //father->setSelected(true);
          for ( int i = 0; i < qtwi->childCount(); i += 1)
            {
              qtwi->child(i)->setSelected(true);
            }
        }
      else
        {
          litestructs[lsInd].unselectField(fieldName);
          bool stillselection=false;
          for ( int i = 0; i < father->childCount(); i += 1)
            {
              if(father->child(i)->isSelected())
                {
                  stillselection = true;
                  break;
                }
            }
          //father->setSelected(stillselection);
          for ( int i = 0; i < qtwi->childCount(); i += 1)
            {
              qtwi->child(i)->setSelected(false);
            }
        }
    }
  else
    {//  Step
      int lsInd = ui->treeWidgetFields->indexOfTopLevelItem(father->parent());
      int fieldInd = father->parent()->indexOfChild(father);
      int stepInd = father->indexOfChild(qtwi);
      std::string fieldName = litestructs[lsInd].getFieldName(fieldInd);
      if(qtwi->isSelected())
        {
          litestructs[lsInd].getField(fieldName).selectStep(stepInd);
          litestructs[lsInd].setSelected(true);
          //father->setSelected(true);
          //father->parent()->setSelected(true);
        }
      else
        {
          litestructs[lsInd].getField(fieldName).unselectStep(stepInd);
          bool stillselection=false;
          for ( int i = 0; i < father->childCount(); i += 1)
            {
              if(father->child(i)->isSelected())
                {
                  stillselection = true;
                  break;
                }
            }
          father->setSelected(stillselection);
          stillselection=false;
          for ( int i = 0; i < father->parent()->childCount(); i += 1)
            {
              if(father->parent()->child(i)->isSelected())
                {
                  stillselection = true;
                  break;
                }
            }
          father->parent()->setSelected(stillselection);
          litestructs[lsInd].setSelected(stillselection);
        }
    }
}

//  Show the selection on standard output using std::cout and str methods from MEDGUILiteStruct
void MEDGUIFileContentDial::sendSelectionToDB()
{
  std::vector<ParaMEDMEM::MEDCalculatorBrowserLiteStruct> lt2send2db;
  for (unsigned int i=0;i<litestructs.size();i++)
    {
      unsigned int nbOfFiels=litestructs[i].getNumberOfFields();
      const ParaMEDMEM::MEDCalculatorBrowserLiteStruct& myStruct=litestructs[i];
      for(unsigned int j=0;j<nbOfFiels;j++)
        {
          if(myStruct.getField(j).isAnySelection())
            try
              {
                //lt2send2db.push_back(myStruct.getField(j).getSelectedTimeSteps());
              }
            catch(INTERP_KERNEL::Exception& e)
              {
              }
        }
    }
  _db->appendFieldLT(lt2send2db);
  /*std::cout<<"Current Selection"<<std::endl;
  for (unsigned int i = 0; i < litestructs.size(); i += 1)
  {
    std::cout<<"Lite Struct n°"<<i<<std::endl;
    std::cout<<litestructs[i].str()<<std::endl;
    std::cout<<"--------------------------------"<<std::endl;
    }*/
}

//  Unselec all fields and meshes
void MEDGUIFileContentDial::unselectAll()
{
   for (unsigned int i = 0; i < litestructs.size(); i += 1)
      {
        litestructs[i].unselectAll();
      }
}

//  Show in red the corresponding meshes from a field or a step or a field hoovered by mouse
//  First clean already colored meshes list (color back in black and empty coloredMeshes vector)
//  Then, get the id of the MEDGUILiteStruct corresponding to the currently hovered item
//  Get corresponding meshes as string from the currently hovered item
//  Get the corresponding root of the meshes QTreeWidget from the root of the fields QTreeWidget
//  Color in red meshes of the meshes QTreeWidget if they are from the same file than qtwi and are support of it or of one of its children
void MEDGUIFileContentDial::overfield(QTreeWidgetItem* qtwi,int col)
{

  for (unsigned int i = 0; i < coloredMeshes.size(); i += 1)
  {
    coloredMeshes[i]->setForeground(0,QBrush(Qt::black));
  }
  coloredMeshes = std::vector<QTreeWidgetItem*>();

  int lsInd = -1;
  if(qtwi->parent()){
    if(qtwi->parent()->parent()) lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi->parent()->parent());
    else  lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi->parent());
  }else lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi);

  std::vector<std::string> meshesNames;
  if(qtwi->parent()){
    if(qtwi->childCount()) meshesNames = litestructs[lsInd].getCorrespondingMeshesFromField(qtwi->parent()->indexOfChild(qtwi));
    else meshesNames.push_back(litestructs[lsInd].getField(qtwi->parent()->parent()->indexOfChild(qtwi->parent())).getCorrespondingMeshFromStep(qtwi->parent()->indexOfChild(qtwi)));
  }else meshesNames = litestructs[lsInd].getCorrespondingMeshesFromLS();

  QTreeWidgetItem *lsroot;
  for ( int i = 0; i < ui->treeWidgetMeshes->topLevelItemCount(); i += 1)
  {
    if(ui->treeWidgetMeshes->topLevelItem(i)->text(0).toStdString() == litestructs[lsInd].getName())
      lsroot = ui->treeWidgetMeshes->topLevelItem(i);
  }

  for ( int i = 0; i < lsroot->childCount(); i += 1)
  {
    if(std::find(meshesNames.begin(),meshesNames.end(),lsroot->child(i)->text(0).toStdString()) != meshesNames.end()){
      lsroot->child(i)->setForeground(0,QBrush(Qt::red));
      coloredMeshes.push_back(lsroot->child(i));
    }
  }
}

//  Show the QLineEdit for selecting steps
//  Get the current selected item
//  Create a QLineEdit inside a QDialog without borders
//  Connect editingFinished signal from QLineEdit to close slot from QDialog
//  Move the QDialog so it will appear next to the current selected item
//  Show the dialog
//  Call the correct select method according to the fact that the current select item is a file, a field or a step
void MEDGUIFileContentDial::selStepsPopup(bool checkable)
{

  QTreeWidgetItem* qtwi = ui->treeWidgetFields->currentItem();

  QDialog *qd = new QDialog(this,Qt::ToolTip);
  QLineEdit *ql = new QLineEdit(qd);

  connect(ql,SIGNAL(editingFinished()),qd,SLOT(close()));

  qd->resize(120,20);
  qd->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  QRect currentItemRec = ui->treeWidgetFields->visualItemRect(qtwi);
  QPoint topLeft = currentItemRec.topLeft();
  QPoint realPos = ui->treeWidgetFields->mapToGlobal(topLeft);

  qd->move(realPos);

  qd->exec();

  QString sel = ql->text();
  if(ql) delete ql;
  if(qd) delete qd;

  if(!qtwi->parent()) selFromFile(qtwi,sel);
  else if(!qtwi->parent()->parent()) selFromField(qtwi,sel);
  else selFromStep(qtwi,sel);

}

//  Method to set all elements of a MEDGUILiteStruct to (un)selected according to a string (such as "all", "none", ...)
//  3 cases :
//  - you want to select all time steps of all fields, the selection is made directly
//  - you want to unselect all time steps of all fields, the unselection is made directly
//  - the string is neither "all" nor "none", selFromField method is called for each field with the string as second argument
//  Empty string means no change at all
void MEDGUIFileContentDial::selFromFile(QTreeWidgetItem* qtwi, QString sel)
{
  if(sel != QString("")){
    if(sel == QString("all")){
      qtwi->setSelected(true);
      litestructs[ui->treeWidgetFields->indexOfTopLevelItem(qtwi)].selectAllFields();
      for ( int i = 0; i < qtwi->childCount(); i += 1)
      {
        qtwi->child(i)->setSelected(true);
        for ( int j = 0; j < qtwi->child(i)->childCount(); j += 1)
        {
          qtwi->child(i)->child(j)->setSelected(true);
        }
      }
    }else if(sel == QString("none")){
      qtwi->setSelected(false);
      litestructs[ui->treeWidgetFields->indexOfTopLevelItem(qtwi)].unselectAllFields();
      for ( int i = 0; i < qtwi->childCount(); i += 1)
      {
        qtwi->child(i)->setSelected(false);
        for ( int j = 0; j < qtwi->child(i)->childCount(); j += 1)
        {
          qtwi->child(i)->child(j)->setSelected(false);
        }
      }
    }else{
        for ( int j = 0; j < qtwi->childCount(); j += 1)
        {
          selFromField(qtwi->child(j),sel);
        }
    }
  }
}

//  Method to set all elements of a MEDGUIField to (un)selected according to a string (such as "all", "none", ...)
//  If empty string, no change are made
//  Else :
//   First get the file and field id
//   Then if statement is all, select all steps, set father to selected
//   If statement is none, unselect all steps, if father get no more child selected, set father to unselected
//   Else
//    Remove all spaces from the string
//    split the string using ";" char (";" means end of command)
//     For each element, split the string with ":" ("a-b" means interval between a and b)
//      If juste one element, select step with this id;
//      If two elements, select all step between the first and the second, if the first is none, min = 0, if the second is none, max = steps.size()
//      Set field to selected
//    Set file to selected
void MEDGUIFileContentDial::selFromField(QTreeWidgetItem* qtwi, QString sel)
{
  if(sel != QString("")){

    int lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi->parent());
    int fieldInd = qtwi->parent()->indexOfChild(qtwi);
    std::string fieldName = litestructs[lsInd].getFieldName(fieldInd);

    if(sel == QString("all")){
      qtwi->setSelected(true);
      litestructs[lsInd].selectField(fieldName);
      qtwi->parent()->setSelected(true);
      for ( int i = 0; i < qtwi->childCount(); i += 1)
        {
           qtwi->child(i)->setSelected(true);
        }

    }else if(sel == QString("none")){
      qtwi->setSelected(false);
      litestructs[lsInd].unselectField(fieldName);
      bool stillselection=false;
      for ( int i = 0; i < qtwi->parent()->childCount(); i += 1)
      {
        if(qtwi->parent()->child(i)->isSelected()){
          stillselection = true;
          break;
        }
      }
      qtwi->parent()->setSelected(stillselection);
      for ( int i = 0; i < qtwi->childCount(); i += 1)
      {
        qtwi->child(i)->setSelected(false);
      }

    }else{
      if(sel.size()!=0){
        sel = sel.simplified();
        QStringList listSel = sel.split(" ");
        sel = listSel.join("");

        listSel = sel.split(";");
        for (int i = 0; i < listSel.size(); i += 1)
        {
          QStringList totreat = listSel[i].split(":");

          if(totreat[0].toInt() >= qtwi->childCount() || totreat[0].toInt() < 0 || (totreat.size() == 2 && ( totreat[1].toInt() >= qtwi->childCount() ))){
            std::ostringstream oss;
            oss << "Input value incorrect, should be between 0 and ";
            oss << qtwi->childCount()-1;
            oss << ".";
            QMessageBox::warning(this,"Incorrect Input Value",oss.str().c_str());
            return;
          }//  Check values
          if(totreat.size() > 2 || ((totreat[0] != QString("0") && totreat[0] != QString("")) && totreat[0].toInt()==0) || ( totreat.size() == 2 && (totreat[1] != QString("") && totreat[1].toInt()==0))){
            std::ostringstream oss;
            oss << "Input values incorrect (";
            oss << listSel[i].toStdString().c_str();
            oss << ").";
            QMessageBox::warning(this,"Incorrect Input Value",oss.str().c_str());
            return;
          }//  Check type (to filter non-int characters)

          if(totreat.size() == 1){
            litestructs[lsInd].getField(fieldName).selectStep(totreat[0].toInt());
            litestructs[lsInd].setSelected(true);
            qtwi->child(totreat[0].toInt())->setSelected(true);

          }else if(totreat.size() == 2){
            int max = qtwi->childCount()-1;
            if(totreat[1] != QString("")) max = totreat[1].toInt();
            if(max < totreat[0].toInt()){
              std::ostringstream oss;
              oss << "Input values incorrect (";
              oss << listSel[i].toStdString().c_str();
              oss << ").";
              QMessageBox::warning(this,"Incorrect Input Value",oss.str().c_str());
              return;
            }//  Check if interval is correct
            for (int j = totreat[0].toInt(); j <= max; j += 1)
            {
              litestructs[lsInd].getField(fieldName).selectStep(j);
              litestructs[lsInd].setSelected(true);
              qtwi->child(j)->setSelected(true);
            }
          }
        }
        qtwi->setSelected(true);
        qtwi->parent()->setSelected(true);
      }
    }
  }
}

//  Method to set all elements of a MEDGUIField when selection is made from a MEDGUIStep to (un)selected according to a string (such as "all", "none", ...)
//  Calling it on a step is the same as calling it from is father (the corresponding field)
void MEDGUIFileContentDial::selFromStep(QTreeWidgetItem* qtwi,QString sel)
{
  selFromField(qtwi->parent(), sel);
}

//  Show the dialog for selecting components and changing components names
//  Get the current QTreeWidgetItem
//  If its a field or a step, get the field id
//  Call MEDGUISelectComponent dial with the MEDGUILiteStruct and the field id (-1 if none)
void MEDGUIFileContentDial::selCompoPopup(bool checkable)
{
  QTreeWidgetItem* qtwi = ui->treeWidgetFields->currentItem();
  int lsInd=0;
  int fieldInd = -1;
  if(!qtwi->parent()) lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi);
  else if(!qtwi->parent()->parent()){
    lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi->parent());
    fieldInd = qtwi->parent()->indexOfChild(qtwi);
  }
  else{
    lsInd = ui->treeWidgetFields->indexOfTopLevelItem(qtwi->parent()->parent());
    fieldInd = qtwi->parent()->parent()->indexOfChild(qtwi->parent());
  }

  MEDGUISelectComponents selcomp(litestructs[lsInd], this, fieldInd);
  selcomp.exec();
}

