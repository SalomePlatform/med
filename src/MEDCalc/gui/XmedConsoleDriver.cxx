// Copyright (C) 2011-2024  CEA, EDF
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

#include "MEDModule.hxx"
#include "XmedConsoleDriver.hxx"
#include "Utils_SALOME_Exception.hxx"
#include "MEDCommandsHistoryManager_i.hxx"
#include "MEDFactoryClient.hxx"
#include CORBA_CLIENT_HEADER(MED_Gen)
#include CORBA_CLIENT_HEADER(MEDCommandsHistoryManager)
#include <iostream>

XmedConsoleDriver::XmedConsoleDriver(MEDModule* salomeModule)
{
  _salomeModule = salomeModule;
  bool forcePythonConsole = true;
  _pyConsole = _salomeModule->getApp()->pythonConsole(forcePythonConsole);
  if ( !_pyConsole ) {
    const char * msg = "The python console can't be obtained from the SALOME application";
    throw SALOME_Exception(msg);
  }
  _importXmedDone = false;
}

/*!
 * This function sends instructions to the python console to setup the
 * environment for med operations (in particular, it imports the medcalc
 * commands). Even if the function is called twice, then the
 * instructions are sent once.
 */
void XmedConsoleDriver::setup() {

  if ( !_importXmedDone ) {
    QStringList commands;
#ifndef DISABLE_PVVIEWER
    // start PVServer and show render view
    commands+="import pvsimple as pvs";
    commands+="";
    //commands+="pvs.ShowParaviewView()";
#endif
    commands += "import medcalc";
    commands += "medcalc.medconsole.setConsoleGlobals(globals())";
    commands += "import MEDCALC";
    commands += "";
    commands += "from medcalc.medconsole import saveWorkspace, cleanWorkspace";
    commands += "from medcalc.medconsole import putInWorkspace, removeFromWorkspace";
    commands += "from medcalc.medconsole import accessField, view";
    commands += "from medcalc.medconsole import getEnvironment, ls, la";
    commands += "";
    this->exec(commands);
    _importXmedDone = true;
  }
}

/*!
 * This function sends the specified list of commands to the console.
 */
void XmedConsoleDriver::exec(const QStringList& commands) {
  MEDCALC::MEDCommandsHistoryManager_ptr history = MEDFactoryClient::getCommandsHistoryManager();

  QStringListIterator it(commands);
  while (it.hasNext()) {
    const QString& command = it.next();
    _pyConsole->exec(command);
    // store command in history
    history->addCommand(command.toStdString().c_str());
  }
}
