// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : MEDGUISelectComponents.cxx
//  Module : MED
//
#include "MEDGUISelectComponents.h"
#include "ui_MEDGUISelectComponents.h"

#include "MEDCalculatorBrowserLiteStruct.hxx"
#include "MEDCalculatorBrowserField.hxx"

#include <sstream>
#include <string>
#include <iostream>

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>

using namespace MEDCoupling;

//  Constructor
//  Initialize qt part and private var part
//  l is the MEDGUILiteStruct that will be modified, fi is a field id, for the case where one field only is selected
//  First, connect button ok and cancel
//  Then, set min and max for the loop according to fieldInd, this will made the QDialog show juste one field (with id fieldInd) or all fields
//  Then, fill the QDialog with the file name
//    For each field concerned, add the field name
//      For each component, add a QCheckBox and the component name as a modifiable QLineEdit
MEDGUISelectComponents::MEDGUISelectComponents(ParaMEDMEM::MEDCalculatorBrowserLiteStruct& l,QWidget *parent, int fi/*=-1*/) :
  QDialog(parent),
  ui(new Ui::MEDGUISelectComponents),
  ls(&l),
  fieldInd(fi)
{
  ui->setupUi(this);

  connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));// close without applying
  connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(fillSelection()));//  apply
  connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(close()));//  close

  int min,max;
  fieldInd!=-1?min=fieldInd:min=0;
  fieldInd!=-1?max=fieldInd:max=(int)ls->getNumberOfFields()-1;

  QLabel *filename = new QLabel(ls->getName().c_str(),this);
  ui->verticalLayout_2->insertWidget(0,filename);

  for (int i = max; i >= min; i -= 1)
    {

      QGridLayout *fieldLayout = new QGridLayout(this);

      QLabel *fieldname = new QLabel(ls->getField(i).getName().c_str(),this);
      fieldLayout->addWidget(fieldname,0,0,1,0);


      for (int j = ls->getField(i).getComponentsSize()-1; j >= 0; j -= 1)
        {
          QCheckBox *cb = new QCheckBox(this);
          if(ls->getField(i).getSelectedComponents()[j]){
            cb->setCheckState(Qt::Checked);
          }

          std::string compname ="";
          if(ls->getField(i).getComponents()[j] != std::string("-noname-")) compname = ls->getField(i).getComponents()[j];

          QLineEdit *le = new QLineEdit(compname.c_str(),this);

          std::ostringstream chName, leName;
          chName << ls->getName().c_str() << ls->getField(i).getName().c_str() << j << "chkbox";
          leName << ls->getName().c_str() << ls->getField(i).getName().c_str() << j << "namebox";

          cb->setObjectName(QString(chName.str().c_str()));
          le->setObjectName(QString(leName.str().c_str()));
          cb->setAccessibleName(QString(chName.str().c_str()));
          le->setAccessibleName(QString(leName.str().c_str()));

          fieldLayout->addWidget(cb,j+1,0);
          fieldLayout->addWidget(le,j+1,1);
        }

      ui->verticalLayout_2->insertLayout(1,fieldLayout);

    }
}

//  Destructor
MEDGUISelectComponents::~MEDGUISelectComponents()
{
  delete ui;
}

//  Qt specific class
void MEDGUISelectComponents::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

// Fill the MEDGUILiteStruct according to the selection and modification made by user
// First init the min,max for the loop : if equal, just one field will be modified, else all field
// Then for each field, for each component, if there is modification, they are applied (selection, unselection or rename)
void MEDGUISelectComponents::fillSelection()
{
  int min,max;
  fieldInd!=-1?min=fieldInd:min=0;
  fieldInd!=-1?max=fieldInd:max=(int)ls->getNumberOfFields()-1;

  for (int i = max; i >= min; i -= 1) {

    for (unsigned int j = 0; j < ls->getField(i).getComponentsSize(); j += 1)
      {
        std::ostringstream chName, leName;
        chName << ls->getName().c_str() << ls->getField(i).getName().c_str() << j << "chkbox";
        leName << ls->getName().c_str() << ls->getField(i).getName().c_str() << j << "namebox";

        QCheckBox *cb = this->findChild<QCheckBox*>(QString(chName.str().c_str()));
        QLineEdit *le = this->findChild<QLineEdit*>(QString(leName.str().c_str()));

        if (cb->isChecked())
          {
            ls->getField(i).selectComponent(j);
          }else{
          ls->getField(i).unselectComponent(j);
        }
        ls->getField(i).setComponentName(j,le->text().toStdString());
      }
  }
}
