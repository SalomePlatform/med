// Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#include "MEDFactoryClient.hxx"
#include "SALOME_KernelServices.hxx"

namespace MEDFactoryClient {

  /*!
   * This returns a singleton (static) instance of the XMED engine.
   */
  MEDCALC::MEDFactory_ptr getFactory() {
    static MEDCALC::MEDFactory_ptr engine;
    if(CORBA::is_nil(engine)){
      Engines::EngineComponent_var component =
        KERNEL::getLifeCycleCORBA()->FindOrLoad_Component( "FactoryServer","MEDFactory" );
      engine = MEDCALC::MEDFactory::_narrow(component);
    }
    return engine;
  }

  /*!
   * This returns a singleton (static) instance of the XMED data manager.
   */
  MEDCALC::MEDDataManager_ptr getDataManager() {
    static MEDCALC::MEDDataManager_ptr dataManager;
    if(CORBA::is_nil(dataManager)){
      dataManager = getFactory()->getDataManager();
    }
    return dataManager;
  }

  /*!
   * This returns a singleton (static) instance of the XMED calculator.
   */
  MEDCALC::MEDCalculator_ptr getCalculator() {
    static MEDCALC::MEDCalculator_ptr calculator;
    if(CORBA::is_nil(calculator)){
      calculator = getFactory()->getCalculator();
    }
    return calculator;
  }

  /*!
   * This returns a singleton (static) instance of the MED presentation manager.
   */
  MEDCALC::MEDPresentationManager_ptr getPresentationManager() {
    static MEDCALC::MEDPresentationManager_ptr presentationManager;
    if(CORBA::is_nil(presentationManager)){
      presentationManager = getFactory()->getPresentationManager();
    }
    return presentationManager;
  }

  /*!
   * This returns a singleton (static) instance of the MED commands history.
   */
  MEDCALC::MEDCommandsHistoryManager_ptr getCommandsHistoryManager() {
    static MEDCALC::MEDCommandsHistoryManager_ptr commandsHistoryManager;
    if(CORBA::is_nil(commandsHistoryManager)){
      commandsHistoryManager = getFactory()->getCommandsHistoryManager();
    }
    return commandsHistoryManager;
  }

}
