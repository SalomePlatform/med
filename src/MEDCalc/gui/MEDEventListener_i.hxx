// Copyright (C) 2011-2021  CEA/DEN, EDF R&D
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

#ifndef _MED_EVENTLISTENER_I_HXX_
#define _MED_EVENTLISTENER_I_HXX_

#include <Python.h>
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDEventListener)
#include "SALOME_GenericObj_i.hh"

#include "MEDCALCGUI.hxx"

#include <QObject>

// Multiple inheritance requires QObject to be first
class MEDCALCGUI_EXPORT MEDEventListener_i: public QObject,
        public POA_MEDCALC::MEDEventListener,
        public SALOME::GenericObj_i

{
  Q_OBJECT

public:

  //
  // ===========================================================
  // Functions specified in the IDL interface
  // ===========================================================
  //
  void processMedEvent(const MEDCALC::MedEvent & event);

  //
  // ===========================================================
  // Other public functions (non available via CORBA)
  // ===========================================================
  //
  static MEDEventListener_i * getInstance();
  static void release();

signals:
  //void medEventSignal();
  void medEventSignal(const MEDCALC::MedEvent * event);


private:
  MEDEventListener_i();
  ~MEDEventListener_i();

  static MEDEventListener_i * _instance;

};

#endif // _MED_EVENTLISTENER_I_HXX_
