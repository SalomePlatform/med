// Copyright (C) 2015-2022  CEA/DEN, EDF R&D
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

#ifndef DLGALIAS_H
#define DLGALIAS_H

#include "ui_DlgAlias.h"
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgAlias : public QDialog
{
  Q_OBJECT

 public:
  DlgAlias(QDialog *parent = 0);

  void setAlias(QString alias);
  QString getAlias();

  private slots:
    void accept();
  //void reject();

  private:
    Ui_DlgAlias ui; // instance of the class defined in ui_DlgAlias.h
};


#endif // DLGALIAS_H
