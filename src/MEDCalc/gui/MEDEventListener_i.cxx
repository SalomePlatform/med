// Copyright (C) 2011-2023  CEA, EDF
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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#include "MEDEventListener_i.hxx"
#include <Basics_Utils.hxx>

MEDEventListener_i * MEDEventListener_i::_instance = NULL;
MEDEventListener_i * MEDEventListener_i::getInstance() {
  // _GBO_ we will certainly need to define one EventListener per
  // DataManager or SALOME study and not one singleton for the whole
  // session
  if ( _instance == NULL ) {
    _instance = new MEDEventListener_i();
  }
  return _instance;
}

void MEDEventListener_i::release() {
  delete _instance;
}

MEDEventListener_i::MEDEventListener_i()
{
  LOG("Creating a MEDEventListener_i instance");
}
MEDEventListener_i::~MEDEventListener_i()
{
  LOG("Deleting MEDEventListener_i instance");
}

void MEDEventListener_i::processMedEvent(const MEDCALC::MedEvent & event) {
  LOG("Start processing event for field id="<<event.dataId);

  // This function must be executed as fast as possible because the
  // CORBA request is a synchronous call. Then we just emit a Qt
  // signal and exit. The Qt signal should be received by the
  // WorkspaceController (connect to a slot of WorkspaceController).

  emit medEventSignal(new MEDCALC::MedEvent(event));
}
