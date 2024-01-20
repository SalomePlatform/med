// Copyright (C) 2015-2024  CEA, EDF
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

#include "GenericDialog.hxx"

GenericDialog::GenericDialog(QDialog *parent) : QDialog(parent)
{
  ui.setupUi(this); // A faire en premier
  
  /*
    Personnalisez vos widgets ici si nécessaire
    Réalisez des connexions supplémentaires entre signaux et slots
  */
  
  // The slots accept() and reject() are already connected to the
  // buttonbox (inherited features) 
}

QFrame * GenericDialog::getPanel() {
  return ui.centralPanel;
}

QDialogButtonBox * GenericDialog::getButtonBox() {
  return ui.buttonBox;
}


#include <QDebug>
void GenericDialog::accept() {
  qDebug() << "accept() is not implemented yet";
  QDialog::accept();
}
