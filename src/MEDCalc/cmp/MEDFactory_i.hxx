// Copyright (C) 2007-2023  CEA, EDF
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

#ifndef _MEDFactory_HXX_
#define _MEDFactory_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDFactory)
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDCalculator)
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include CORBA_SERVER_HEADER(MEDCommandsHistoryManager)
#include "SALOME_Component_i.hxx"

#include "MEDCALC.hxx"
class MEDCALC_EXPORT MEDFactory_i: public POA_MEDCALC::MEDFactory,
                                   public Engines_Component_i
{

public:

  MEDFactory_i(CORBA::ORB_ptr orb,
               PortableServer::POA_ptr poa,
               PortableServer::ObjectId * contId,
               const char *instanceName,
               const char *interfaceName,
			   bool checkNS = true);
  virtual ~MEDFactory_i();

  /*! Returns the singleton instance of the data manager */
  MEDCALC::MEDDataManager_ptr getDataManager();
  /*! Returns the singleton instance of the calculator */
  MEDCALC::MEDCalculator_ptr getCalculator();
  /*! Returns the singleton instance of the presentation manager */
  MEDCALC::MEDPresentationManager_ptr getPresentationManager();
  /*! Returns the singleton instance of the commands history manager */
  MEDCALC::MEDCommandsHistoryManager_ptr getCommandsHistoryManager();
};

#endif
