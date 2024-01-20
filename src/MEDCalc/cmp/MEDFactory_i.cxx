// Copyright (C) 2007-2024  CEA, EDF
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

#include "MEDFactory_i.hxx"
#include "utilities.h"

#include "MEDDataManager_i.hxx"
#include "MEDCalculator_i.hxx"
#include "MEDPresentationManager_i.hxx"
#include "MEDCommandsHistoryManager_i.hxx"

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================

MEDFactory_i::MEDFactory_i(CORBA::ORB_ptr orb,
                           PortableServer::POA_ptr poa,
                           PortableServer::ObjectId* contId,
                           const char* instanceName,
                           const char* interfaceName,
						   bool checkNS)
  : Engines_Component_i(orb, poa, contId, instanceName, interfaceName, false, checkNS)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
}

MEDFactory_i::~MEDFactory_i()
{
  // nothing to do
}

MEDCALC::MEDDataManager_ptr
MEDFactory_i::getDataManager()
{
  MEDDataManager_i* medDataManager = MEDDataManager_i::getInstance();
  MEDCALC::MEDDataManager_ptr medDataManagerPtr = medDataManager->_this();
  return medDataManagerPtr;
}

MEDCALC::MEDCalculator_ptr
MEDFactory_i::getCalculator()
{
  MEDCalculator_i* medCalculator = MEDCalculator_i::getInstance();
  MEDCALC::MEDCalculator_ptr medCalculatorPtr = medCalculator->_this();
  return medCalculatorPtr;
}

MEDCALC::MEDPresentationManager_ptr
MEDFactory_i::getPresentationManager()
{
  MEDPresentationManager_i* manager = MEDPresentationManager_i::getInstance();
  MEDCALC::MEDPresentationManager_ptr managerPtr = manager->_this();
  return managerPtr;
}

MEDCALC::MEDCommandsHistoryManager_ptr
MEDFactory_i::getCommandsHistoryManager()
{
  MEDCommandsHistoryManager_i* manager = MEDCommandsHistoryManager_i::getInstance();
  MEDCALC::MEDCommandsHistoryManager_ptr managerPtr = manager->_this();
  return managerPtr;
}
