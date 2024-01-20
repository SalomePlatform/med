// Copyright (C) 2012-2024  CEA, EDF
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

//
// This program can be used for unit test of dialog box.
// You just have to include the interface file (*.h) and
// use the dialog class as illustrated in the functions TEST_*.
// (gboulant - 12/10/2010)
//
#include <QApplication>

#include "DlgAlias.hxx"

#include <iostream>
using std::cout;
using std::endl;
#define LOG(msg) cout << "[XMED - dlgTester]::"<< msg << endl;



void TEST_dlgAlias_show(int argc, char *argv[]) { 
  QApplication app(argc,argv);

  DlgAlias dialog;
  dialog.setAlias("vitesse");
  dialog.show();

  app.exec();
}

void TEST_dlgAlias_exec(int argc, char *argv[]) {
  QApplication app(argc,argv);

  DlgAlias dialog;
  QString alias = "vitesse";
  dialog.setAlias(alias);
  int choice = dialog.exec();
  if ( choice == QDialog::Accepted ) {
    LOG("Clic on OK");
  }
  else {
    LOG("Clic on Cancel");
  }

  //app.exec();
}


int main(int argc, char *argv[])
{
  
  //TEST_dlgAlias_show(argc, argv);
  TEST_dlgAlias_exec(argc, argv);
  
}
